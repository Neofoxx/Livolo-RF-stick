#include <p32xxxx.h>    // Always in first place to avoid conflict with const.h -> this includes xc.h
#include <const.h>      // MIPS32
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <newlib.h>
#include <errno.h>
#include <stdbool.h>

// Drivers for HW
#include <configBits.h>
#include <GPIODrv.h>
#include <UARTDrv.h>
#include <SPIDrv.h>
#include <BTN.h>
#include <LED.h>
#include <helper.h>
#include <RFM69.h>


// These numbers are for a baud of 57142bps.
#define LIVOLO_PREAMBLE_LEN_MIN	(32-2)
#define LIVOLO_PREAMBLE_LEN_MAX	(32+2)
#define LIVOLO_ONE_LEN_MIN	(19-2)
#define LIVOLO_ONE_LEN_MAX	(19+2)
#define LIVOLO_ZERO_LEN_MIN	(9-2)
#define LIVOLO_ZERO_LEN_MAX	(9+2)


volatile char tempArray[2560];
volatile uint32_t lengthArray = 0;
uint8_t retBuffer[8];

void simpleDelay(unsigned int noOfLoops){
    unsigned int i = 0;
    while (i<noOfLoops){
        i++;
        asm("nop");
    }
}

void setup(){

	


	SYSKEY = 0x00000000;      // force lock
	SYSKEY = 0xAA996655;      // unlock
	SYSKEY = 0x556699AA; 

	SPLLCON = (0b000 << 24)	// PLLODIV = PLL divide-by-1, so we get 24MHZ from 24MHz
		| (0b0000001 << 16)		// PLLMULT = x3, so 8MHz * 3 = 24MHz
		| (0b1 << 7);			// FRC is selected as input to PLL


//	SPLLCON = (0b010 << 24)	// PLLODIV = PLL divide-by-4, so we get 24MHZ from 96MHz
//	| (0b0000101 << 16)		// PLLMULT = x12, so 8MHz * 12 = 96MHz
//	| (0b1 << 7);			// FRC is selected as input to PLL


	simpleDelay(1000);


	OSCCONbits.NOSC = 1;	// Switch to SPLL
	
	OSCCONSET = 1;			// Sets the OSWEN bit, that forces a change to news settings in NOSC
	
	SYSKEY = 0x00000000;      // force lock

	simpleDelay(100000);

	
	LED_init();
	BTN_init();
	UARTDrv_Init(115200);
	
	
	
	SPIDrv_Init(1000000, 1, 0, 0, SPI_MODE_NORMAL);	// 1MHz, CKE = 1, CKP = 0, SMP = 0, normal SPI mode.
	RFM69_Init();
	
	
	// Enable interrupts
//	INTEnableSystemMultiVectoredInt();
	
	//	_CP0_BIS_CAUSE(_CP0_CAUSE_IV_MASK);

	// Enable multi-vectored mode
//	INTCONSET = _INTCON_MVEC_MASK;

	// set the CP0 status IE bit high to turn on interrupts
	//INTEnableInterrupts();
//	asm("ei");	// Compiler handles this, but should be same as _CP0_BIS_STATUS(_CP0_STATUS_IE_MASK)

	
}

void rssiSniffer(){
	// Try to roll through frequencies, get RSSI.
	uint32_t startFreq = 430000000;
	uint32_t stopFreq = 435000000;
	uint32_t step = 10000;
	int16_t rssiCurr = -128;
	int16_t rssiTemp = -128;
	uint32_t counter = 0;
	uint32_t numCounts = 10;

	int16_t maxRSSI = -128;
	int32_t maxFreq = 0;

	for (; startFreq <= stopFreq; startFreq = startFreq + step){
		rssiCurr = -128;
		rssiTemp = -128;
		RFM69_SetRfCarrierFreq(startFreq);
		for (counter = 0; counter < numCounts; counter++){
			rssiTemp = RFM69_GetRSSI();
			if (rssiTemp > rssiCurr){
				rssiCurr = rssiTemp;
			}
		}
		
		if (rssiCurr > maxRSSI){
			maxRSSI = rssiCurr;
			maxFreq = startFreq;
		}
		
		lengthArray = sprintf(tempArray, "RSSI @ %dkHz is %ddBm\n", startFreq/1000, rssiCurr);
		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);    
		
				
	}

	lengthArray = sprintf(tempArray, "Peak RSSI was %ddBm @ %dkHz\n", maxRSSI, maxFreq/1000);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);   

	delayms(5000);
}

void tryCapture(){

	uint32_t startTime = _CP0_GET_COUNT();
	uint32_t totalCount = (((uint64_t)FCLK/(uint64_t)2) * (uint64_t)5000) / 1000;	

	lengthArray = sprintf(tempArray, "Packet Capture procedure. Waiting for RSSI > -70dBm\n");
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);

	// Perform for x seconds
	while((_CP0_GET_COUNT() - startTime) <= totalCount){

		int16_t tempRssi = RFM69_GetRSSI();

		if (tempRssi > -90){
			lengthArray = sprintf(tempArray, "RSSI is good (%ddBm), proceeding\n", tempRssi);
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);	
			
			SPIDrv_Init(57142, 1, 0, 0, SPI_MODE_RX_OTHER_SDI);
			
			// One packet is 8ms, or 57B. Double to be sure to get something, +%
			uint8_t suggestiveData[128] = {0};
			SPIDrv_SendReceiveBlocking(suggestiveData, suggestiveData, 128);
			
			/*
			uint32_t counter = 0;
			int8_t bitCounter = 0;
			lengthArray = 0;
			for (counter = 0; counter<128; counter++){
				for (bitCounter = 7; bitCounter>0; bitCounter--){
					lengthArray = lengthArray + sprintf(&tempArray[lengthArray], "%d", (suggestiveData[counter] & (1<<bitCounter)) ? 1 : 0);
				}
				//lengthArray = lengthArray + sprintf(&tempArray[lengthArray], "%02x", suggestiveData[counter]);
				lengthArray = lengthArray + sprintf(&tempArray[lengthArray], ",");
			}
			lengthArray = lengthArray + sprintf(&tempArray[lengthArray], "\nDONE\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			*/
			
			// Procedure, find needs in haystack (preamble)
			// Then decode 24bits (varying width), probably a 0 at the end but we'll see
			
			// Get next pattern - startPosition BYTE, startPosition BIT, stopPosition BYTE.
			// Should return (bitVal, numOfBits, and end).
			
			uint32_t bitPos = 1;
			uint32_t bitPosLimit = sizeof(suggestiveData) * 8;
			
			uint32_t done = 0;
			
			// Data should be MSB FIRST!
			
			uint8_t prevBit = (suggestiveData[0] >> 7) & 0x01;
			uint32_t currBitCounter = 1;
			
			bool preambleFound = false;
			uint32_t data = 0;
			uint8_t dataCounter = 0;
			uint8_t shortCounter = 0;
			
			while (!done && bitPos<bitPosLimit){
				uint32_t whichByte = bitPos >> 3;			// 3 bits from 0-7 are bit pos, other are bytes.
				uint32_t whichBit = 7 - (bitPos & 0b111);	// Bit pos
				uint8_t currBit = (suggestiveData[whichByte] >> whichBit) & 0x01;	
				
				if (currBit == prevBit){
					currBitCounter++;
				}
				else{
					//lengthArray = sprintf(tempArray, "Bit val %d, length %d\n", prevBit, currBitCounter);
    				//UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
    				
    				// Discern here
    				
					if (currBitCounter >= LIVOLO_PREAMBLE_LEN_MIN && currBitCounter <= LIVOLO_PREAMBLE_LEN_MAX){
						preambleFound = true;
						data = 0;
						dataCounter = 0;
					}
    					
					else if (preambleFound == true){
						// If one
						
						if (currBitCounter >= LIVOLO_ONE_LEN_MIN && currBitCounter <= LIVOLO_ONE_LEN_MAX){
							data = (data << 1) | 1;
							shortCounter = 0;
							dataCounter++;
						}
						else if (currBitCounter >= LIVOLO_ZERO_LEN_MIN && currBitCounter <= LIVOLO_ZERO_LEN_MAX){
							shortCounter++;
							if (shortCounter == 2){
								shortCounter = 0;
								data = (data << 1);
								dataCounter++;
							}
						}
						else{
							// Invalid data.
							preambleFound = false;
							data = 0;
							dataCounter = 0;
						}
						
    				}
    				
    				else{
    					// Not valid bit.
    					preambleFound = false;
						data = 0;
						dataCounter = 0;
    				}
    				
    				if (dataCounter == 23){
    					done = true;
    				}
    				
    				prevBit = currBit;
    				currBitCounter = 1;
				}
			
				bitPos++;
			}

			if (done){			
				uint16_t remote = (data >> 8) & 0xFFFF;
				uint8_t keycode = data & 0x7F;
				//lengthArray = sprintf(tempArray, "Bit val %d, length %d\n", prevBit, currBitCounter);
				lengthArray = sprintf(tempArray, "RemoteID %d, KeycodeID %d\n", remote, keycode);
				UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			}
			else{
				lengthArray = sprintf(tempArray, "No luck\n");
				UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			}
			
			SPIDrv_Init(1000000, 1, 0, 0, SPI_MODE_NORMAL);		// Revert back to normal
		}
	
	}
	
	lengthArray = sprintf(tempArray, "Timeout, procedure end");
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
	
}

void sendPacket(uint16_t id, uint8_t keycode, uint16_t numTimes){
	// Prepare SPI out buffer first
	uint8_t outData[64];
	uint8_t lenData = 0;
	uint8_t bitPos = 7;
	
	memset(outData, 0, sizeof(outData));
	
	// Preamble is a LONG pulse.
	// Subsequent pulses are two short pulses for 0, or 1 long pulse for 1.

	uint32_t counter = 0;
	uint32_t counterInner = 0;
	uint32_t nextVal = 0;
	
	for (counter = 0; counter < (LIVOLO_PREAMBLE_LEN_MIN + LIVOLO_PREAMBLE_LEN_MAX) / 2; counter++){
		outData[lenData] = outData[lenData] | (1<<bitPos);
		bitPos--;
		if (bitPos > 7){
			lenData++;
			bitPos = 7;
		}
	}
	
	for (counter = 0; counter < 16; counter++){
		if (id & (1<<(15-counter))){
			for (counterInner = 0; counterInner < (LIVOLO_ONE_LEN_MIN + LIVOLO_ONE_LEN_MAX) / 2; counterInner++){
				outData[lenData] = outData[lenData] | (nextVal<<bitPos);
				bitPos--;
				if (bitPos > 7){
					lenData++;
					bitPos = 7;
				}
			}
			
			nextVal = (nextVal + 1) & 0x01;
					
		}
		else{
			for (counterInner = 0; counterInner < (LIVOLO_ZERO_LEN_MIN + LIVOLO_ZERO_LEN_MAX) / 2; counterInner++){
				outData[lenData] = outData[lenData] | (nextVal<<bitPos);
				bitPos--;
				if (bitPos > 7){
					lenData++;
					bitPos = 7;
				}
			}
			
			nextVal = (nextVal + 1) & 0x01;
			
			for (counterInner = 0; counterInner < (LIVOLO_ZERO_LEN_MIN + LIVOLO_ZERO_LEN_MAX) / 2; counterInner++){
				outData[lenData] = outData[lenData] | (nextVal<<bitPos);
				bitPos--;
				if (bitPos > 7){
					lenData++;
					bitPos = 7;
				}
			}	
			
			nextVal = (nextVal + 1) & 0x01;
		}
	}
	
	for (counter = 0; counter < 7; counter++){
		if (keycode & (1<<(7-counter))){
			for (counterInner = 0; counterInner < (LIVOLO_ONE_LEN_MIN + LIVOLO_ONE_LEN_MAX) / 2; counterInner++){
				outData[lenData] = outData[lenData] | (nextVal<<bitPos);
				bitPos--;
				if (bitPos > 7){
					lenData++;
					bitPos = 7;
				}
			}
			
			nextVal = (nextVal + 1) & 0x01;
					
		}
		else{
			for (counterInner = 0; counterInner < (LIVOLO_ZERO_LEN_MIN + LIVOLO_ZERO_LEN_MAX) / 2; counterInner++){
				outData[lenData] = outData[lenData] | (nextVal<<bitPos);
				bitPos--;
				if (bitPos > 7){
					lenData++;
					bitPos = 7;
				}
			}
			
			nextVal = (nextVal + 1) & 0x01;
			
			for (counterInner = 0; counterInner < (LIVOLO_ZERO_LEN_MIN + LIVOLO_ZERO_LEN_MAX) / 2; counterInner++){
				outData[lenData] = outData[lenData] | (nextVal<<bitPos);
				bitPos--;
				if (bitPos > 7){
					lenData++;
					bitPos = 7;
				}
			}	
			
			nextVal = (nextVal + 1) & 0x01;
		}
	}
	
	// There should be an extra 1 at the end, to give the decoder an edge to latch on to (all pun intended)
	for (counterInner = 0; counterInner < (LIVOLO_ONE_LEN_MIN + LIVOLO_ONE_LEN_MAX) / 2; counterInner++){
		outData[lenData] = outData[lenData] | (nextVal<<bitPos);
		bitPos--;
		if (bitPos > 7){
			lenData++;
			bitPos = 7;
		}
	}
	
	nextVal = (nextVal + 1) & 0x01;
	
	
	// Done.
	
	
	lengthArray = sprintf(tempArray, "Wrote to outData: %d %d\n", lenData, bitPos);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
	
	for (counter = 0; counter < (lenData+1); counter++){
		lengthArray = sprintf(tempArray, "0x%X ", outData[counter]);
		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
	}
	lengthArray = sprintf(tempArray, "\n");
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);


	// Config RFM for TXing
	uint8_t temp = RFM69_ReadReg(RFM69_RegOpMode);
	RFM69_SetReg(RFM69_RegOpMode, ((0b011 << RFM69_RegOpMode_Bit_Shift_Mode) & RFM69_RegOpMode_Bit_Mask_Mode) );		// Switch to TX mode, automatically.
	
	while(!(RFM69_ReadReg(RFM69_RegIrqFlags1) & RFM69_RegIrqFlags1_Bit_Mask_ModeReady)){
	}
	
	SPIDrv_Init(57142, 1, 0, 0, SPI_MODE_TX_OTHER_SDO);
	

	// We will write 1B more - it's just 0s anyway
	for (counter = 0; counter < numTimes; counter++){		
		SPIDrv_SendBlocking(outData, lenData + 1);
	}
	
	
	SPIDrv_Init(1000000, 1, 0, 0, SPI_MODE_NORMAL);		// Revert back to normal
	RFM69_SetReg(RFM69_RegOpMode, temp);
	while(!(RFM69_ReadReg(RFM69_RegIrqFlags1) & RFM69_RegIrqFlags1_Bit_Mask_ModeReady)){
	}


}

int main(){

	bool printout = true;

	setup();


    for(;;){
    
    	lengthArray = sprintf(tempArray, "Waiting %d 0x%X 0x%X\n", BTN_getStatus(), PORTA, CNPUA);
		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
		LED_setRed(1);

    	if (BTN_getStatus()){
    		printout = !printout;
    	}

    	
    	if(printout){
    		LED_setGreen(1);
    		uint8_t temp;
    		
  		
    		// TEMPERATURE
//    		lengthArray = sprintf(tempArray, "\nTrying to read RFM69 temperature... ");
//    		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
    		
//    		temp = RFM69_GetTemp();
//    		lengthArray = sprintf(tempArray, "Read %d, probably around %dC\n", temp, 170-temp);
//    		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
  		
    		
    		// REVISION
    		lengthArray = sprintf(tempArray, "Trying to read RFM69 Revision...\n");
    		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
    		
    		RFM69_GetRevision(retBuffer);
    		lengthArray = sprintf(tempArray, "Revision %d, Mask rev. %d\n", retBuffer[0], retBuffer[1]);
    		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
    		
    		lengthArray = sprintf(tempArray, "\nStatus register is %02x\n", temp);
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			
			temp = RFM69_ReadReg(RFM69_RegIrqFlags2);
			lengthArray = sprintf(tempArray, "Other status register is %02x\n", temp);
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);   
			
			// RSSI
    		lengthArray = sprintf(tempArray, "Trying to read Current RSSI... ");
    		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
    		
    		int16_t int_temp = RFM69_GetRSSI();
    		lengthArray = sprintf(tempArray, "RSSI is supposed to be %ddBm\n", int_temp);
    		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);		
    		
    		tryCapture();
    		
    		

    		
    		
/*		
    		while(1){
				// Status reg
				temp = RFM69_ReadReg(RFM69_RegIrqFlags1);
				lengthArray = sprintf(tempArray, "\nStatus register is %02x\n", temp);
				UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
				
				temp = RFM69_ReadReg(RFM69_RegIrqFlags2);
				lengthArray = sprintf(tempArray, "Other status register is %02x\n", temp);
				UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
				
				temp =  RFM69_ReadReg(RFM69_RegRssiValue);
				lengthArray = sprintf(tempArray, "RSSI val raw %d %0x\n", temp, RFM69_ReadReg(RFM69_RegRssiConfig));
				UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
				
				temp = RFM69_ReadReg(RFM69_RegTemp1);
				lengthArray = sprintf(tempArray, "T1 raw %d\n", temp);
				UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
				
				temp = RFM69_ReadReg(RFM69_RegTemp2);
				lengthArray = sprintf(tempArray, "T2 raw %d\n", temp);
				UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
				
				if (BTN_getStatus()){
					
					RFM69_SetReg(RFM69_RegTemp1, RFM69_RegTemp1_Bit_Mask_TempMeasureStart);
					lengthArray = sprintf(tempArray, "SET\n", temp);
					UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
					
    				RFM69_SetReg(RFM69_RegRssiConfig, RFM69_RegRssiConfig_Bit_Mask_RssiStart);	// Trigger an RSSI measurement
    				lengthArray = sprintf(tempArray, "SET\n");
					UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
					
					RFM69_SetReg(RFM69_RegPacketConfig2, RFM69_RegPacketConfig2_Bit_Mask_RestartRx);	// Trigger an RSSI measurement
    				
				}
				
				delayms(100);
			}

*/


    		
    		
    		
    		
    				
    	}
    	else{
    		LED_setGreen(0);
    		
    		sendPacket(25984, 68<<1, 100);
    	}
   	
    	
    	
    	
    	delayms(250);
    	delayms(250);
    	delayms(250);
    	
    	    	
    }

/**********************************************************************/

    return(0);
    
} // end of main


