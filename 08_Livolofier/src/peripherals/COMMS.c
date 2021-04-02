#include <p32xxxx.h>
#include <inttypes.h>
#include <COMMS.h>
#include <string.h>
#include <stdio.h>


comStruct comStruct_UART_RX = {0};	// PC to us


// HELPER FUNCTION
// helper function to add to buffer, so don't have to deal with hard coded things etc.
uint32_t COMMS_helper_addToBuf(comStruct* st, uint8_t* data, uint16_t length){
	if (COMMS_helper_spaceLeft(st) < length){
		return 1;	// Fail
	}

	uint32_t i = 0;
	for (i=0; i < length; i++){
		st->data[st->head] = data[i];
		st->head = (st->head + 1) & cyclicBufferSizeMask;
	}

	return 0; // 0 on success, else otherwise (no space available)
}

// Returns how much data is in the struct
uint32_t COMMS_helper_dataLen(comStruct* st){
	return (st->head - st->tail) & cyclicBufferSizeMask;
}

// Returns how much space is left in the struct
uint32_t COMMS_helper_spaceLeft(comStruct* st){
	return (st->tail - st->head - 1) & cyclicBufferSizeMask;
}

// Get x bytes
void COMMS_helper_getData(comStruct* st, uint8_t *buf, uint16_t length){
	if (COMMS_helper_dataLen(st) < length){
		// Don't do this please, check beforehand
		return;
	}
	uint32_t i = 0;
	for (i=0; i< length; i++){
		buf[i] = st->data[st->tail];
		st->tail = (st->tail + 1) & cyclicBufferSizeMask;
	}
}

// "Delete" any data in the buffer
void COMMS_helper_dropAll(comStruct* st){
	st->tail = st->head;
}

// Check, if a character/value is present
uint32_t COMMS_helper_charPresent(comStruct* st, uint8_t val, uint32_t *len){
	if (!COMMS_helper_dataLen(st)){
		return 0;
	}
	
	uint32_t found = 0;
	uint32_t lenFound = 0;
	uint32_t start = st->tail;
	uint32_t stop = st->head;
	while(start != stop){
		lenFound = lenFound + 1;
		if (st->data[start] == val){
			found = 1;
			if (len){
				*len = lenFound;
			}
			break;
		}
		start = (start + 1) & cyclicBufferSizeMask;
	}
	
	return found;
}


