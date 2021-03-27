#include <p32xxxx.h>
#include <inttypes.h>
#include <LED.h>
#include <GPIODrv.h>


void LED_init(){
	LEDUSER_TRISbits.LEDUSER_TRISPIN = 0;	// Set to output
}

void LED_setState(uint8_t state){
	if (state){
		LEDUSER_LATbits.LEDUSER_LATPIN = 1;
	}
	else{
		LEDUSER_LATbits.LEDUSER_LATPIN = 0;
	}
}

void LED_toggle(){
	LEDUSER_LATINV = LEDUSER_MASK;
}
