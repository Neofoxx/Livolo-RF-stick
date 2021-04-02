#include <p32xxxx.h>
#include <inttypes.h>
#include <LED.h>
#include <GPIODrv.h>


void LED_init(){
	LEDGREEN_TRISbits.LEDGREEN_TRISPIN = 0;	// Set to output
	LEDRED_TRISbits.LEDRED_TRISPIN = 0;	// Set to output
}

void LED_setGreen(uint8_t state){
	if (state){
		LEDGREEN_LATbits.LEDGREEN_LATPIN = 1;
	}
	else{
		LEDGREEN_LATbits.LEDGREEN_LATPIN = 0;
	}
}

void LED_setRed(uint8_t state){
	if (state){
		LEDRED_LATbits.LEDRED_LATPIN = 1;
	}
	else{
		LEDRED_LATbits.LEDRED_LATPIN = 0;
	}
}

void LED_toggle(){
	LEDGREEN_LATINV = LEDGREEN_MASK;
	LEDRED_LATINV = LEDRED_MASK;
}
