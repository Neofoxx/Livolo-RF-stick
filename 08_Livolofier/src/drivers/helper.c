#include <p32xxxx.h>
#include <helper.h>
#include <inttypes.h>

// Delays in us, up to about 90s
void delayus(uint32_t useconds){
	// Ticks in CP0 ticks
	uint32_t totalCount = (((uint64_t)FCLK/(uint64_t)2) * (uint64_t)useconds) / 1000000;	
	uint32_t startTime = _CP0_GET_COUNT();
	
	// Loop until current time - start time is bigger than number of cycles
	while ((_CP0_GET_COUNT() - startTime) <= totalCount){
	}
}

// Delays in ms, up to about 90s
void delayms(uint32_t mseconds){
	// Ticks in CP0 ticks
	uint32_t totalCount = (((uint64_t)FCLK/(uint64_t)2) * (uint64_t)mseconds) / 1000;	
	uint32_t startTime = _CP0_GET_COUNT();
	
	// Loop until current time - start time is bigger than number of cycles
	while ((_CP0_GET_COUNT() - startTime) <= totalCount){
	}
}
