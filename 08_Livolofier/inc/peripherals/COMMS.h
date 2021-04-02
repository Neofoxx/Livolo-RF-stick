#ifndef COMMS_H_693a835f02eb4197bff3cc252edc4036
#define COMMS_H_693a835f02eb4197bff3cc252edc4036


#define cyclicBufferSize		128
#define cyclicBufferSizeMask	(cyclicBufferSize-1)
// Check to enforce buffer being of size 2^n
_Static_assert( ((cyclicBufferSize & cyclicBufferSizeMask) == 0), "Buffer size not equal to 2^n");

typedef struct comStruct{
	uint8_t data[cyclicBufferSize];
	uint16_t head;
	uint16_t tail;
} comStruct;

extern comStruct comStruct_UART_RX;	// PC to us


uint32_t COMMS_helper_addToBuf(comStruct* st, uint8_t* data, uint16_t length);
uint32_t COMMS_helper_dataLen(comStruct* st);
uint32_t COMMS_helper_spaceLeft(comStruct* st);
void COMMS_helper_getData(comStruct* st, uint8_t *buf, uint16_t length);
void COMMS_helper_dropAll(comStruct* st);
uint32_t COMMS_helper_charPresent(comStruct* st, uint8_t val, uint32_t *len);

#endif
