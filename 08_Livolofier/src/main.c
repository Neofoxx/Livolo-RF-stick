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
#include <COMMS.h>
#include <helper.h>
#include <RFM69.h>


// These numbers are for a baud of 57142bps.
#define LIVOLO_PREAMBLE_LEN_MIN	(32-2)
#define LIVOLO_PREAMBLE_LEN_MAX	(32+2)
#define LIVOLO_ONE_LEN_MIN	(19-2)
#define LIVOLO_ONE_LEN_MAX	(19+2)
#define LIVOLO_ZERO_LEN_MIN	(9-2)
#define LIVOLO_ZERO_LEN_MAX	(9+2)


char tempArray[512];
uint32_t lengthArray = 0;


void simpleDelay(unsigned int noOfLoops){
    unsigned int i = 0;
    while (i<noOfLoops){
        i++;
        asm("nop");
    }
}

void MIPS32 INTEnableSystemMultiVectoredInt(void)
{
    uint32_t val;

    // set the CP0 cause IV bit high
    asm volatile("mfc0   %0,$13" : "=r"(val));
    val |= 0x00800000;
    asm volatile("mtc0   %0,$13" : "+r"(val));

    INTCONSET = _INTCON_MVEC_MASK;

    // set the CP0 status IE bit high to turn on interrupts
    //INTEnableInterrupts();
	asm("ei");
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


	// set the CP0 status IE bit high to turn on interrupts
	//INTEnableInterrupts();
	//asm("ei");	// Compiler handles this, but should be same as _CP0_BIS_STATUS(_CP0_STATUS_IE_MASK)

	// As per datasheet
	unsigned int temp_CP0;
	asm volatile("di"); 
	asm volatile("ehb");
	temp_CP0 = _CP0_GET_CAUSE();
	temp_CP0 |= 0x00800000;
	_CP0_SET_CAUSE(temp_CP0);
	INTCONSET = _INTCON_MVEC_MASK;
	
	asm volatile("ei"); 
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
	
	LED_setGreen(1);

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
		
		lengthArray = sprintf(tempArray, "RSSI @ %ldkHz is %ddBm\n", startFreq/1000, rssiCurr);
		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);    
		
				
	}

	lengthArray = sprintf(tempArray, "Peak RSSI was %ddBm @ %ldkHz\n", maxRSSI, maxFreq/1000);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);   
	
	LED_setGreen(0);


	RFM69_Init();	// Need to set back to proper frequency, and this is the quickest.
}

void tryCapture(int16_t time, int16_t minRssi){

	uint32_t toRun = true;

	uint32_t startTime = _CP0_GET_COUNT();
	uint32_t totalCount = (((uint64_t)FCLK/(uint64_t)2) * (uint64_t)time) / 1000;	

	LED_setGreen(1);

	lengthArray = sprintf(tempArray, "Packet Capture procedure. Waiting for RSSI >= %ddBm\n", minRssi);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);

	// Perform for x seconds
	while(toRun){

		int16_t tempRssi = RFM69_GetRSSI();

		if (tempRssi >= minRssi){
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
					
					// Should check if we're checking the last bit (dataCounter == 22)
					// .....
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
						else if (dataCounter == 22){
							// When _we_ transmit, the last bit can be a bit longer...
							// We can asume that it is a 0, if shortCounter == 1, else assume 1
							if (shortCounter == 1){
								shortCounter = 0;
								data = (data << 1);
								dataCounter++;
							}
							else{
								data = (data << 1) | 1;
								shortCounter = 0;
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
				uint16_t remote = (data >> 7) & 0xFFFF;
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
	
	
	
		// Check exit conditions
		// Either timeout (when > 0), or any new character in buffer (non-exclusive)
		if (time > 0){
			if ((_CP0_GET_COUNT() - startTime) >= totalCount){
				toRun = 0;
			}
		}
		
		if (COMMS_helper_dataLen(&comStruct_UART_RX)){
			toRun = 0;
		}
	
	}
	
	LED_setGreen(0);
	
	lengthArray = sprintf(tempArray, "Read end\n");
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
	
	LED_setRed(1);
	
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
		if (keycode & (1<<(6-counter))){
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
	
	// The data HAS to end on a 0.
	
	// Should there be an extra 1 at the end, to give the decoder an edge to latch on to? (all pun intended)
	// Maybe, need to check if a Livolo devices accepts it or not
	// -> If it doesn't then the SPI thing won't be completely OK for this,
	// -> and a simple "if timer overflown then set bit = x" will be better...
	// We could also... copy the last sent byte to the _end_ of the array. And rotate. Coyote laying the tracks style
	// 'cos you gotta know, the original remote is fucky. Sometimes it drops a bit at the end, so I suspect fimilar fuckery.
	
	// So, turns out, that the Livolo switches __really__ need that next edge to latch on to.
	// Without it (say another zero half period), it doesn't budge. So no period, or prolonged last bit == no bueno.
	// This sucks. It also means we'll have to use a more simple TX-ing mechanism with timed delays.
	
/*	
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
	
*/	

	uint32_t combinedData = (uint32_t)(keycode & 0x7F) | (((uint32_t)(id)) << 7);
	
	uint32_t startTime;
	uint32_t uSecondTicks = ((uint64_t)FCLK/(uint64_t)2) / 1000000;
	uint32_t countPreamble = uSecondTicks * 560;	// Preamble is a nominal 560us high
	uint32_t countHigh = uSecondTicks * 325;	// High is a nominal 325us high (could also be 315us, or 330us)
	uint32_t countLow = uSecondTicks * 157;		// Low is a nominal 2 * 157us low
	nextVal = 0;
		

	uint8_t temp = RFM69_ReadReg(RFM69_RegOpMode);
	RFM69_SetReg(RFM69_RegOpMode, ((0b011 << RFM69_RegOpMode_Bit_Shift_Mode) & RFM69_RegOpMode_Bit_Mask_Mode) );		// Switch to TX mode, automatically.
	
	while(!(RFM69_ReadReg(RFM69_RegIrqFlags1) & RFM69_RegIrqFlags1_Bit_Mask_ModeReady)){
	}
	
	GPIODrv_initSpecialPin(GPIO_MODE_OUTPUT);
	
	// TX here	
	for (counter = 0; counter < numTimes; counter++){
		// Set pin
		GPIODrv_setSpecialPin(true);	
		// Wait
		startTime = _CP0_GET_COUNT();
		while ((_CP0_GET_COUNT() - startTime) < countPreamble){
		}
		
		nextVal = 0;
	
		for (counterInner = 0; counterInner < 23; counterInner++){
			if (combinedData & (1<<(22-counterInner))){
				// Set pin
				GPIODrv_setSpecialPin(nextVal);
				
				nextVal = (nextVal + 1) & 0x01;
				// Wait
				startTime = _CP0_GET_COUNT();
				while ((_CP0_GET_COUNT() - startTime) < countHigh){
				}
			}
			else{
				// Set pin
				GPIODrv_setSpecialPin(nextVal);
				
				nextVal = (nextVal + 1) & 0x01;
				
				startTime = _CP0_GET_COUNT();
				while ((_CP0_GET_COUNT() - startTime) < countLow){
				}
				
				// Set pin
				GPIODrv_setSpecialPin(nextVal);
				
				nextVal = (nextVal + 1) & 0x01;
				
				startTime = _CP0_GET_COUNT();
				while ((_CP0_GET_COUNT() - startTime) < countLow){
				}
			}		
		}
	
	}
	
	GPIODrv_initSpecialPin(GPIO_MODE_INPUT);
	
	RFM69_SetReg(RFM69_RegOpMode, temp);
	while(!(RFM69_ReadReg(RFM69_RegIrqFlags1) & RFM69_RegIrqFlags1_Bit_Mask_ModeReady)){
	}
	
	LED_setRed(0);

}

void toUpper(char *buf, uint32_t len){
	uint32_t counter = 0;
	for (counter = 0; counter < len; counter++){
		if (buf[counter] >= 97 && buf[counter] <= 122){
			buf[counter] = buf[counter] - 32;
		}
	}
}

void printDebug(){
	uint8_t retBuffer[8];
	uint8_t temp;

	// REVISION
	lengthArray = sprintf(tempArray, "RFM69 Stats & specs...\n");
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
	
	RFM69_GetRevision(retBuffer);
	lengthArray = sprintf(tempArray, "Revision %d, Mask rev. %d\n", retBuffer[0], retBuffer[1]);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
	
	temp = RFM69_ReadReg(RFM69_RegIrqFlags1);
	lengthArray = sprintf(tempArray, "\nStatus register is %02x\n", temp);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
	
	temp = RFM69_ReadReg(RFM69_RegIrqFlags2);
	lengthArray = sprintf(tempArray, "Other status register is %02x\n", temp);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);   
	
	// RSSI
	int16_t int_temp = RFM69_GetRSSI();
	lengthArray = sprintf(tempArray, "Current RSSI is %ddBm\n", int_temp);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);	
	
	//TEMPERATURE
	temp = RFM69_GetTemp();
	lengthArray = sprintf(tempArray, "Temperature raw is %d, probably around %d°C\n", temp, 170-temp);
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
	
	lengthArray = sprintf(tempArray, "Button status: %d\n", BTN_getStatus());
	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);	
}

int main(){

	//bool printout = true;
	uint32_t lenPos = 0;
	char myBuffer[cyclicBufferSize];
	
	char command[17];
	int32_t operands[4];

	setup();


    for(;;){
/*    
    	lengthArray = sprintf(tempArray, "Looping %ld\n", COMMS_helper_dataLen(&comStruct_UART_RX));
    	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
   
   		lengthArray = sprintf(tempArray, "Vals 0x%08X 0x%08X 0x%08X 0x%08X\n", IFS1, IEC1, IPC10, INTCON);
    	UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
*/    
    
    
    	// Find out, if we need to go:
    	// - TX /w param (remoteId, keyCode, numTransmits)
    	// - RX /w param (time?, rssi?)
    	
    	if (COMMS_helper_charPresent(&comStruct_UART_RX, '\n', &lenPos)){
    		COMMS_helper_getData(&comStruct_UART_RX, (uint8_t *)myBuffer, lenPos);
    	}
    	else if (!COMMS_helper_spaceLeft(&comStruct_UART_RX)){
    		// STOP SPAMMING
    		COMMS_helper_dropAll(&comStruct_UART_RX);
    		continue;
    	}
    	else{
    		continue;
    	}
    	
    	toUpper(myBuffer, lenPos);
    	
    	sscanf(myBuffer, "%16s", command);
    	
    	if (0 == strcmp(command, "SNIFF")){
    		sscanf(myBuffer, "%16s %ld %ld", command, &operands[0], &operands[1]);
    		if (operands[0] < -115 || operands[0] > 0			// Min RSSI
    			|| operands[1] < 0 || operands[1] > 15000){		// time to sniff. 0 == forever
    			// Bork & return
    			continue;
    		}
    		
    		tryCapture(operands[1], operands[0]);
    	}
    	
    	else if (0 == strcmp(command, "TRANSMIT")){
    		// Minimum number of transmits for "on/off" should be ~30. Use 50. Don't use more than 100, or it'll turn itself off :D
    		// For "dim up", 20 seems to be a minimum. There is a limit to the discretization the switch does
    		
    		// For pairing, use 30. More than that and it'll unpair itself :p
    		// -> While the manual doesn't says this, on the remote, if you start synchronization (one "Dit"),
    		// -> Then press a buttons (second "Dit"), this synchronizes/binds the button the the dwitch
    		// -> If you press a button _again_ (THIRD "Dit"), it will unpair/unbind it!
    		// -> Hende why a 50 here will cause trouble. Fun tho, and you don't have to unlearn everything.
    	
    		sscanf(myBuffer, "%16s %ld %ld %ld", command, &operands[0], &operands[1], &operands[2]);
    		if (operands[0] < 0 || operands[0] >= 65535			// Remote id is 16-bit
    			|| operands[1] < 0 || operands[1] >= 127		// Keycode is 7-bit
    			|| operands[2] < 0 || operands[2] >= 1001){		// 1000 repeats is 8 seconds
    			// Bork & return
    			continue;
    		}
    		
    		sendPacket(operands[0], operands[1], operands[2]);
    	}   
    	
    	else if (0 == strcmp(command, "DEBUG")){
    		printDebug();
    	}
    	
    	else if (0 == strcmp(command, "SCAN")){
    		rssiSniffer();	// Fixed frequencies is fine for now.
    	}
    	
    	else if (0 == strcmp(command, "HELP")){
    		lengthArray = sprintf(tempArray, "RF stick, for controlling Livolo switches\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			
			lengthArray = sprintf(tempArray, "Commands:\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			
			lengthArray = sprintf(tempArray, "HELP - Print this help\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			
			lengthArray = sprintf(tempArray, "SCAN - Perform an RSSI scan of a wider area\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			
			lengthArray = sprintf(tempArray, "DEBUG - Print out some diagnostic data\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			
			lengthArray = sprintf(tempArray, "SNIFF [Minimum RSSI] [Timeout] - Sniff for Livolo packets.\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			lengthArray = sprintf(tempArray, "-> RSSI should be [-115 to 0]. In practice, use -80, maybe -90 for really weak signals\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			lengthArray = sprintf(tempArray, "-> Timeout is in miliseconds. You can put 0 for no timeout. You can always end by sending \\n\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			
			lengthArray = sprintf(tempArray, "TRANSMIT [Remote ID] [Keycode] [Number of repeats] - Send a packet many times\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			lengthArray = sprintf(tempArray, "-> Remote ID is a 16-bit unsigned number\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			lengthArray = sprintf(tempArray, "-> Keycode is a 6-bit unsigned number\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			lengthArray = sprintf(tempArray, "-> Number of packets can be [1 to 1000]. Each one takes 8ms, fyi.\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			lengthArray = sprintf(tempArray, "----> THIS DOES NOT VERIFY IF YOU DATA IS CORRECT. YOU HAVE TO MAKE SURE THAT YOU HAVE THE RIGHT NUMBER OF 1s!\n");
			UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
			
			
    	}
    	    

/*    
    	lengthArray = sprintf(tempArray, "Waiting %d 0x%X 0x%X\n", BTN_getStatus(), PORTA, CNPUA);
		UARTDrv_SendBlocking((uint8_t *) tempArray, lengthArray);
		LED_setRed(1);



    	if (BTN_getStatus()){
    		printout = !printout;
    	}

	
    	if(printout){
    		LED_setGreen(1);
    		uint8_t temp;
   				
    	}
    	else{
    		LED_setGreen(0);
    		
    		sendPacket(25984, 68<<1, 100);
    	}
    	
    	delayms(250);
    	delayms(250);
    	delayms(250);
    	
*/    	    	
    }

/**********************************************************************/

    return(0);
    
} // end of main


