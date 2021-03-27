#include <p32xxxx.h>    // Always in first place to avoid conflict with const.h -> this includes xc.h
#include <system.h>     // System setup
#include <const.h>      // MIPS32
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <newlib.h>
#include <errno.h>
#include <stdbool.h>

// Drivers for HW
#include <GPIODrv.h>
#include <UARTDrv.h>
#include <SPIDrv.h>
#include <BTN.h>
#include <LED.h>
#include <helper.h>
#include <RFM69.h>

const uint32_t __attribute__((section (".SECTION_DEVCFG3"))) temp3 = 
	0xFFF0000
	| (0b1 << _DEVCFG3_FVBUSONIO_POSITION)	// USBVBUSON controlled by USB module
	| (0b0 << _DEVCFG3_FUSBIDIO_POSITION)	// USBID controlled by PORT function
	| (0b0 << _DEVCFG3_IOL1WAY_POSITION)		// Allow multiple reconfigurations of Peripheral Pins
	| (0b0 << _DEVCFG3_PMDL1WAY_POSITION)	// Allow multiple reconfigurations of Peripheral Module Disable
	| (0xF0C5 << _DEVCFG3_USERID_POSITION);	// UserID is F0C5

const uint32_t __attribute__((section (".SECTION_DEVCFG2"))) temp2 =
	0xFFF87888
	| (0b001 << _DEVCFG2_FPLLODIV_POSITION)	// PLL output divided by 2 (96MHz/2 = 48Mhz)
	| (0b0 << _DEVCFG2_UPLLEN_POSITION)		// USB PLL Enabled
	| (0b001 << _DEVCFG2_UPLLIDIV_POSITION)	// USB PLL input divided by 2 (8MHz/2 = 4MHz)
	| (0b111 << _DEVCFG2_FPLLMUL_POSITION)	// PLL Multiplier is 24 (4MHz*24 = 96MHz)
	| (0b001 << _DEVCFG2_FPLLIDIV_POSITION);	// PLL input divided by 2 (8MHz/2 = 4MHz)

const uint32_t __attribute__((section (".SECTION_DEVCFG1"))) temp1 =
	0xFC200858
	| (0b00 << _DEVCFG1_FWDTWINSZ_POSITION)	// Watchdog timer window size is 75%
	| (0b0 << _DEVCFG1_FWDTEN_POSITION)		// Watchdog timer disabled, can be enabled in software
	| (0b00000 << _DEVCFG1_WDTPS_POSITION)	// Watchdog timer postscale is 1
	| (0b01 << _DEVCFG1_FCKSM_POSITION)		// Clock switching enabled, Fail-Safe Clock Monitoring DISABLED
	| (0b01 << _DEVCFG1_FPBDIV_POSITION)	// PBCLK is SYSCLK / 2
	| (0b1 << _DEVCFG1_OSCIOFNC_POSITION)	// CLOCK output disabled
	| (0b01 << _DEVCFG1_POSCMOD_POSITION)	// XT ocillator mode
	| (0b0 << _DEVCFG1_IESO_POSITION)		// Internal-External switchover disabled (Two-speed start-up disabled)
	| (0b1 << _DEVCFG1_FSOSCEN_POSITION)		// Enable secondary oscillator (WARNING; CHECK IF PORTING)
	| (0b011 << _DEVCFG1_FNOSC_POSITION);		// POSC (XT) + PLL selected

const uint32_t __attribute__((section (".SECTION_DEVCFG0"))) temp0 =
	0x6EF803E0								// Don't forget about that one 0
	| (0b1 << _DEVCFG0_CP_POSITION)			// Code Protection disabled
	| (0b1 << _DEVCFG0_BWP_POSITION)			// Boot Flash is Writeable during code execution
	| (0b1111111111 << _DEVCFG0_PWP_POSITION)	// Memory is NOT write-protected
	| (0b00 << _DEVCFG0_ICESEL_POSITION)		// PGEC4/PGED4 is used
	| (0b1 << _DEVCFG0_JTAGEN_POSITION)		// JTAG is enabled
#ifdef DEBUG_BUILD							// Defined with Makefile
	| (0b11<<_DEVCFG0_DEBUG_POSITION);		// Debugger is DISABLED. Apparently the MX1/MX2 family need this disabled, for JTAG to work
											// Note, application will run automatically, might want to add a delay at the beginning.
#else
	| (0b11<<_DEVCFG0_DEBUG_POSITION);		// Debugger is DISABLED (DEBUG bit) - DEBUG NEEDS TO BE DISABLED, IF NO DEBUGGER PRESENT! Otherwise code doesn't run.
#endif

volatile char tempArray[2560];
volatile uint32_t lengthArray = 0;
uint8_t retBuffer[8];

void setup(){
	// What is the equivalent of SYSTEMConfigPerformance?
	// -> Setting up the system for the required System Clock
	// -> Seting up the Wait States
	// -> Setting up PBCLK
	// -> Setting up Cache module (not presenf on MX1/2, but is on MX4)
	// Also of interest: https://microchipdeveloper.com/32bit:mx-arch-exceptions-processor-initialization
	// See Pic32 reference manual, for CP0 info http://ww1.microchip.com/downloads/en/devicedoc/61113e.pdf

	// DO NOT setup KSEG0 (cacheable are) on MX1/MX2, debugging will NOT work

	BMXCONbits.BMXWSDRM = 0;	// Set wait-states to 0
	
	// System config, call with desired CPU freq. and PBCLK divisor
	SystemConfig(48000000L, 2);	// Set to 48MHz, with PBCLK with divider 1 (same settings as DEVCFG)

	//UARTDrv_Init(115200);

	LED_init();
	BTN_init();
	UARTDrv_Init(115200);
	
	SPIDrv_Init(1000000, 1, 0, 0, SPI_MODE_NORMAL);	// 1MHz, CKE = 1, CKP = 0, SMP = 0, normal SPI mode.
	RFM69_Init();
	
	// Enable interrupts
	INTEnableSystemMultiVectoredInt();
	
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

		if (tempRssi > -70){
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
			
// These numbers are for a baud of 57142bps.
#define LIVOLO_PREAMBLE_LEN_MIN	(32-2)
#define LIVOLO_PREAMBLE_LEN_MAX	(32+2)
#define LIVOLO_ONE_LEN_MIN	(19-2)
#define LIVOLO_ONE_LEN_MAX	(19+2)
#define LIVOLO_ZERO_LEN_MIN	(9-2)
#define LIVOLO_ZERO_LEN_MAX	(9+2)


			
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

int main(){

	bool printout = false;

	setup();


    for(;;){
    	if (BTN_getStatus()){
    		printout = !printout;
    	}
    	
    	if(printout){
    		LED_setState(1);
    		uint8_t temp;
    		
/*  		
    		// TEMPERATURE
    		lengthArray = sprintf(tempArray, "\nTrying to read RFM69 temperature... ");
    		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
    		
    		temp = RFM69_GetTemp();
    		lengthArray = sprintf(tempArray, "Read %d, probably around %dC\n", temp, 170-temp);
    		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
*/  		
    		
    		// REVISION
    		lengthArray = sprintf(tempArray, "Trying to read RFM69 Revision... ");
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
    		LED_setState(0);
    	}
    	
    	
    	
    	
    	delayms(250);
    	    	
    }

/**********************************************************************/

    return(0);
    
} // end of main


