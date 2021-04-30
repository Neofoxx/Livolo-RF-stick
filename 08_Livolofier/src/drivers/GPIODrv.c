#include <p32xxxx.h>
#include <GPIODrv.h>

void GPIODrv_initSpecialPin(uint32_t mode){
	// Output
	if (mode == 0){
		SPI_DIO2_TRISbits.SPI_DIO2_TRISPIN = 0;
	}
	// Input, Hi-z, don't care
	else{
		SPI_DIO2_TRISbits.SPI_DIO2_TRISPIN = 1;
	}
	
}

void GPIODrv_setSpecialPin(bool state){
	// Can make faster with SET and CLR registers. Meh, later
	if (state){
		SPI_DIO2_LATbits.SPI_DIO2_LATPIN = 1;
	}
	else{
		SPI_DIO2_LATbits.SPI_DIO2_LATPIN = 0;
	}
}
