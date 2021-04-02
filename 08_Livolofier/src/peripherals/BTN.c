#include <p32xxxx.h>
#include <inttypes.h>
#include <GPIODrv.h>

void BTN_init(){
	// One button on RA3
	BTN_ANSELbits.BTN_ANSELPIN = 0;
	BTN_TRISbits.BTN_TRISPIN = 1;					// Set as input
	BTN_PULLREG = BTN_PULLREG | BTN_PULLBIT;	// Enable pull-up (in this case)
}

void BTN_update(){
	// TODO use if updating via SysTick or something
}

uint8_t BTN_getStatus(){
	// Inverted logic - button pressed gives 0
	if (BTN_PORTbits.BTN_PORTPIN){
		return 0;
	}
	else{
		return 1;
	}

}
