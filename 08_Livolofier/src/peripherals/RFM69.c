#include <p32xxxx.h>
#include <RFM69.h>
#include <SPIDrv.h>
#include <GPIODrv.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>
#include <helper.h>

#include <UARTDrv.h>
uint8_t sanityBuffer[128];
uint16_t sanityLength = 0;

void RFM69_setReset(bool value){
	// Reset is active high
	if (value){
		RFM_RESET_LATSET = RFM_RESET_MASK;
	}
	else{
		RFM_RESET_LATCLR = RFM_RESET_MASK;
	}
}

void RFM69_SetReg(uint8_t reg, uint8_t val){
	uint8_t buffer[2] = {reg | 0x80, val};	// Set high bit in reg, to write 
	
	SPI_NSS_LATCLR = SPI_NSS_MASK;		// Assert NSS
	SPIDrv_SendBlocking(buffer, 2);
	SPI_NSS_LATSET = SPI_NSS_MASK;		// Deassert NSS
}

uint8_t RFM69_ReadReg(uint8_t reg){
	uint8_t buffer[2] = {reg & 0x7F, 0};	// Clear high bit in reg, to read
	SPI_NSS_LATCLR = SPI_NSS_MASK;			// Assert NSS
	SPIDrv_SendReceiveBlocking(buffer, buffer, 2);	// Reuse buffer
	SPI_NSS_LATSET = SPI_NSS_MASK;			// Deassert NSS
	return buffer[1];
}

uint8_t RFM69_GetTemp(){
	// Can only be triggered in Standby or FS mode ><
	// First switch to FS, then read, then restore mode.
	
	uint8_t temp = RFM69_ReadReg(RFM69_RegOpMode);

	RFM69_SetReg(RFM69_RegOpMode, ((0b010 << RFM69_RegOpMode_Bit_Shift_Mode) & RFM69_RegOpMode_Bit_Mask_Mode) );		// Switch to FS mode, automatically.
	
	while(!(RFM69_ReadReg(RFM69_RegIrqFlags1) & RFM69_RegIrqFlags1_Bit_Mask_ModeReady)){
	}
	
	RFM69_SetReg(RFM69_RegTemp1, RFM69_RegTemp1_Bit_Mask_TempMeasureStart);
	while(RFM69_ReadReg(RFM69_RegTemp1) & RFM69_RegTemp1_Bit_Mask_TempMeasureRunning){
	}
	
	uint8_t temp2 = RFM69_ReadReg(RFM69_RegTemp2);
	RFM69_SetReg(RFM69_RegOpMode, temp);

	while(!(RFM69_ReadReg(RFM69_RegIrqFlags1) & RFM69_RegIrqFlags1_Bit_Mask_ModeReady)){
	}
	
	return temp2;
}

// 1st byte is full rev number, 2nd is mask number
void RFM69_GetRevision(uint8_t * outBuf){
	uint8_t temp = RFM69_ReadReg(RFM69_RegVersion);
	outBuf[0] = (temp & RFM69_RegVersion_Bit_Mask_FullRevNumber) >> RFM69_RegVersion_Bit_Shift_FullRevNumber;
	outBuf[1] = (temp & RFM69_RegVersion_Bit_Mask_MetalMaskRev) >> RFM69_RegVersion_Bit_Shift_MetalMaskRev;
}

void RFM69_SetBitrate(uint32_t bitrate){
	uint32_t rate = RFM69_FXOSC / bitrate;
	RFM69_SetReg(RFM69_RegBitrateMsb, (rate & 0xFF00) >> 8);
	RFM69_SetReg(RFM69_RegBitrateLsb, (rate & 0xFF));	
}

void RFM69_SetFreqDeviation(uint32_t deviation){
	// register = deviation / FSTEP
	uint32_t dev = deviation / RFM69_FSTEP;
	RFM69_SetReg(RFM69_RegFdevMsb, (dev & 0x3F00) >> 8);	// MSB has only 6 bits
	RFM69_SetReg(RFM69_RegFdevLsb, (dev & 0xFF));
}

void RFM69_SetRfCarrierFreq(uint32_t centerFreq){
	// register = freq / FSTEP
	uint32_t freq = (uint32_t)((float)centerFreq / RFM69_FSTEP);
	RFM69_SetReg(RFM69_RegFrfMsb, (freq & 0xFF0000) >> 16);
	RFM69_SetReg(RFM69_RegFrfMid, (freq & 0xFF00) >> 8);
	RFM69_SetReg(RFM69_RegFrfLsb, (freq & 0xFF));
}

// Output power in dBm, gtom -18dBm to +20dBm. Not every power is available on every chip...
void RFM69_SetOutputPower(int32_t power){
	// For RFM69HCW, PA0 is not available
	// PA1 goes from -2 to +13dBm (even though it says -18dBm, internet says it is no bueno)
	// PA1+PA2 goes from +2 to +17dBm
	// PA1+PA2 /w 20dBm setting goes from +18dBm to +20dBm
	// -> 20dBm setting must be DISABLED in RX MODE
	uint8_t regVal = 0;
	
	if (power < -2 || power > 20){
		for(;;);
	}
	else if (power <= 13){
		regVal = regVal | RFM69_RegPaLevel_Bit_Mask_Pa1On | ((+18 + power) & RFM69_RegPaLevel_Bit_Mask_OutputPower);
	}
	else if (power <= 17){
		regVal = regVal | RFM69_RegPaLevel_Bit_Mask_Pa1On | RFM69_RegPaLevel_Bit_Mask_Pa2On | ((+14 + power) & RFM69_RegPaLevel_Bit_Mask_OutputPower);		
	}
	else{
		for(;;); // Deal with it later, it's not legal in EU anyway...
	}
	
	RFM69_SetReg(RFM69_RegPaLevel, regVal);
	
}

void RFM69_SetOverCurrentProtection(bool enabled, uint8_t current){
	// Imax = 45mA + 5mA * regVal
	// Min is 45mA, max is 120mA, MAXMAX is OCP disabled
	if (current>120){
		current = 120;
	}
	if (current<45){
		current = 45;
	}
	
	uint8_t val = (enabled ? RFM69_RegOcp_Bit_Mask_OcpOn : 0) | (45 - (current / 5));
	
	RFM69_SetReg(RFM69_RegOcp, val);
}

void RFM69_SetLNA(bool impedance, uint8_t gain){
	// 0 = 50ohm, 1 = 200ohm
	// gain = 000 for AGC, 110 for highest gain (48dB), 111 is reserved
	uint8_t val = (impedance ? RFM69_RegLna_Bit_Mask_LnaZin : 0) | gain;

	RFM69_SetReg(RFM69_RegLna, val);
}

void RFM69_SetRxBw(bool modulation, uint8_t dccFreq, uint32_t rxBw){
	// modulation = 0 for FSK, 1 for OOK
	// dccFreq = 0 for 16%, 7 for 0.125%
	// rxBw is rx bandwith in kHz. Selects nearest (higher)
	
	int32_t rxBwMantissa = 0;
	int32_t rxBwExponent = 0;
	uint32_t tempBw = 0;
	for (rxBwMantissa = 2; rxBwMantissa >= 0; rxBwMantissa--){
		for(rxBwExponent = 7; rxBwExponent >= 0; rxBwExponent--){
			tempBw = (RFM69_FXOSC) / ((16 + 4 * rxBwMantissa) * pow(2, rxBwExponent + (modulation ? 3 : 2) ));
			if (tempBw >= rxBw){
				break;
			}
		}
		if (tempBw >= rxBw){
			break;
		}
	}	
	
	// In case of no match, set highest BW.
	if (rxBwMantissa < 0 || rxBwExponent < 0){
		rxBwMantissa = 0;
		rxBwExponent = 0;
	}
	
	// Limiting done in for loop
	uint8_t val = (dccFreq << RFM69_RegRxBw_Bit_Shift_DccFreq) 
		| (rxBwMantissa << RFM69_RegRxBw_Bit_Shift_RxBwMant)
		| (rxBwExponent << RFM69_RegRxBw_Bit_Shift_RxBwExp);

	RFM69_SetReg(RFM69_RegRxBw, val);	
}

int16_t RFM69_GetRSSI(){
	RFM69_SetReg(RFM69_RegRssiConfig, RFM69_RegRssiConfig_Bit_Mask_RssiStart);	// Trigger an RSSI measurement
	while(! (RFM69_ReadReg(RFM69_RegRssiConfig) & RFM69_RegRssiConfig_Bit_Mask_RssiDone)){
	}
	
	int16_t temp = RFM69_ReadReg(RFM69_RegRssiValue) / 2;
	temp = temp * -1;
	
	return temp;
}

void RFM69_SetDioMapping(uint8_t dio, uint8_t map){
	// Get register, clear bits, set bits
	uint8_t tempReg = 0;
	if (dio >= 0 && dio <= 3){
		tempReg = RFM69_ReadReg(RFM69_RegDioMapping1);
		switch(dio){
			case 0:	tempReg = tempReg & ~RFM69_RegDioMapping1_Bit_Mask_Dio0Mapping; 
					tempReg = tempReg | (map << RFM69_RegDioMapping1_Bit_Shift_Dio0Mapping);
			break;
			case 1:	tempReg = tempReg & ~RFM69_RegDioMapping1_Bit_Mask_Dio1Mapping; 
					tempReg = tempReg | (map << RFM69_RegDioMapping1_Bit_Shift_Dio1Mapping);
			break;
			case 2:	tempReg = tempReg & ~RFM69_RegDioMapping1_Bit_Mask_Dio2Mapping; 
					tempReg = tempReg | (map << RFM69_RegDioMapping1_Bit_Shift_Dio2Mapping);
			break;
			case 3:	tempReg = tempReg & ~RFM69_RegDioMapping1_Bit_Mask_Dio3Mapping; 
					tempReg = tempReg | (map << RFM69_RegDioMapping1_Bit_Shift_Dio3Mapping);
			break;	
		}
		RFM69_SetReg(RFM69_RegDioMapping1, tempReg);
	}
	else if (dio >= 4 && dio <= 5){
		tempReg = RFM69_ReadReg(RFM69_RegDioMapping2);
		switch(dio){
			case 4:	tempReg = tempReg & ~RFM69_RegDioMapping2_Bit_Mask_Dio4Mapping; 
					tempReg = tempReg | (map << RFM69_RegDioMapping2_Bit_Shift_Dio4Mapping);
			break;
			case 5:	tempReg = tempReg & ~RFM69_RegDioMapping2_Bit_Mask_Dio5Mapping; 
					tempReg = tempReg | (map << RFM69_RegDioMapping2_Bit_Shift_Dio5Mapping);
			break;
		}
		RFM69_SetReg(RFM69_RegDioMapping2, tempReg);
	}	
}

void RFM69_SetCLKOUT(uint8_t val){
	uint8_t tempReg = RFM69_ReadReg(RFM69_RegDioMapping2);
	tempReg = tempReg & ~RFM69_RegDioMapping2_Bit_Mask_ClkOut; 
	tempReg = tempReg | ((val << RFM69_RegDioMapping2_Bit_Shift_ClkOut) & RFM69_RegDioMapping2_Bit_Mask_ClkOut);
	RFM69_SetReg(RFM69_RegDioMapping2, tempReg);
}



void RFM69_Init(){
	// Reset procedure says to wait 100us, pulse RESET high for 100us, then release and wait for 5ms
	RFM69_setReset(0);
	delayus(120);
	RFM69_setReset(1);
	delayus(120);
	RFM69_setReset(0);
	delayms(6);

	// Initialization of registers goes here.

	
		
	RFM69_SetReg(RFM69_RegDataModul, 
		((0b11 << RFM69_RegDataModul_Bit_Shift_DataMode) & RFM69_RegDataModul_Bit_Mask_DataMode)					// Continuous mode, without bit synchronizer
		| ((0b01 << RFM69_RegDataModul_Bit_Shift_ModulationType) & RFM69_RegDataModul_Bit_Mask_ModulationType));	// OOK modulation
	// No shaping of data
	
	//RFM69_SetBitrate(5780);	// 5780bps works for Livolo RX
	//RFM69_SetBitrate(1200);	// 1.2kbps doesn't work for Livolo RX
	RFM69_SetBitrate(13000);	// 13kbps works for Livolo RX (maybe even better?)
	
	RFM69_SetFreqDeviation(5000);	// 5kHz deviation, shouldn't matter in OOK
	//RFM69_SetFreqDeviation(50000);	// 5kHz deviation, shouldn't matter in OOK
	
	RFM69_SetRfCarrierFreq(433920000);	// Remote should be at 433.92 according to design doc -> And it is

	// Don't need to trigger RC calibration
	
	// Don't need The Listen registers
	
	RFM69_SetOutputPower(0);	// Set output pwer to 0dBm
	//RFM69_SetOutputPower(13);	// Set output pwer to 0dBm
	
	// Don't need to set PA ramp - only for FSK
	
	RFM69_SetOverCurrentProtection(true, 95);	// Enable OCP and set to 95mA, as per default
	
	RFM69_SetLNA(1, 0);	// Set LNA impedance to 200ohm as per datasheet, leave gain to AGC
	
	//RFM69_SetRxBw(1, 2, 1300);	// OOK modulation, 4% of RxBw, RxBw = 1.3kHz (default, very narrow) - doesn't work for Livolo RX
	RFM69_SetRxBw(1, 2, 13000);	// OOK modulation, 4% of RxBw - works for Livolo RX
	//RFM69_SetRxBw(1, 2, 250000);	// OOK modulation, 4% of RxBw, RxBw = 250kHz (very broad) - works for Livolo RX
	
	
	// AFC is probably AutoFrequencyCorrection? Don't need it

	// RegOokPeak, Avg and Fix - setup later, if OOK decoding is useful

	// FEI is FrequencyErrorIndicator. Might be interesting, but not needed atm.
	
	RFM69_SetDioMapping(2, 0b00);	// Sets DIO2 to output DATA in Continous Mode, RX.
	
	RFM69_SetCLKOUT(0b111);			// Set CLKOUT to off.
	
	// Don't need IRQ Flags 1&2
	
	// Don't need to set RSSI threshold for interrupt
	
	// Don't need to set RX Timeout 1&2
	
	// Nothing to setup in the packet engine
	
	
	RFM69_SetReg(RFM69_RegOpMode, 
	((0b100 << RFM69_RegOpMode_Bit_Shift_Mode) & RFM69_RegOpMode_Bit_Mask_Mode) );		// RX mode
	// Sequencer is ON (auto-walks thourgh STBY->FS->RX), Listen mode disabled, Listen abort disabled


	while(!(RFM69_ReadReg(RFM69_RegIrqFlags1) & RFM69_RegIrqFlags1_Bit_Mask_ModeReady)){
	}

}
