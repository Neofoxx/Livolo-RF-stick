#include <p32xxxx.h>
#include <SPIDrv.h>
#include <GPIODrv.h>
#include <system.h>
#include <inttypes.h>

void SPIDrv_Init(uint32_t clkSpeed, uint8_t edge, uint32_t polarity, uint8_t sample, uint8_t mode){

	// Turn off SPI module
	SPI1CONbits.ON = 0;
	
	// Clear input buffer at this point
	while(SPI1STATbits.SPIRBF == 1){
		volatile uint32_t discard = SPI1BUF;
	}


	// Pin setting. Common first
	// SCK
	SPI_SCK_TRISbits.SPI_SCK_TRISPIN = 0;	// Output
	SPI_SCK_LATCLR = SPI_SCK_MASK;			// Set to some default state
	// NSS
	SPI_NSS_TRISbits.SPI_NSS_TRISPIN = 0;	// Output
	SPI_NSS_LATSET = SPI_NSS_MASK;			// Unselect SPI slave
	// Reset
	RFM_RESET_TRISbits.RFM_RESET_TRISPIN = 0;	// Output
	RFM_RESET_LATCLR = RFM_RESET_MASK;			// Take out of reset

	if (mode == SPI_MODE_NORMAL){
	
		// Unmap other pins.
		// SPI_DIO2_REMAP_REG -> SDI1R, done below
		SPI_DIO2_RP_REG = 0;	// Unmaps the pin from SDO1
	
		// SDI/MISO
		SPI_SDI_TRISbits.SPI_SDI_TRISPIN = 1;	// Input 
		SPI_SDI_PULLREG = SPI_SDI_PULLREG | SPI_SDI_MASK;	// Enable pull-down
		SPI_SDI_REMAP_REG = SPI_SDI_REMAP_VAL;	// Remap pin to SPI module
		// SDO/MOSI
		SPI_SDO_TRISbits.SPI_SDO_TRISPIN = 0;	// Output
		SPI_SDO_RP_REG = SPI_SDO_RP_VAL;		// Remap pin to SPI module
		
		
	}
	else if (mode == SPI_MODE_RX_OTHER_SDI){
		// RX only. Unmap SDO pins, and remap SDI1
	
		// Unmap other pins.
		SPI_SDO_RP_REG = 0;		// Unmaps the pin from SDO1
		SPI_DIO2_RP_REG = 0;	// Unmaps the pin from SDO1
		
		// SDI/MISO
		SPI_DIO2_ANSELbits.SPI_DIO2_ANSELPIN = 0;	// Disable analog input
		SPI_DIO2_TRISbits.SPI_DIO2_TRISPIN = 1;		// Input 
		SPI_DIO2_REMAP_REG = SPI_DIO2_REMAP_VAL;	// Remap pin to SPI module
	
	}
	else if (mode == SPI_MODE_TX_OTHER_SDO){
		// TX only. Rx can be left on the pin, can't unmap it anyway. 
		
		// Unmap other pins.
		SPI_SDO_RP_REG = 0;		// Unmaps the pin from SDO1
		
		// SDO/MOSI
		SPI_DIO2_ANSELbits.SPI_DIO2_ANSELPIN = 0;	// Disable analog input
		SPI_DIO2_TRISbits.SPI_DIO2_TRISPIN = 0;		// Output
		SPI_DIO2_LATbits.SPI_DIO2_LATPIN = 0;		// Set to 0
		SPI_DIO2_RP_REG = SPI_DIO2_RP_VAL;			// Remap pin to SPI module
		
		// SDI/MISO - pin has to be somewhere.
		SPI_DIO2_REMAP_REG = SPI_DIO2_REMAP_VAL;	// Remap pin to SPI module
	
	}
	
	// Set SPI clock frequency
	// PBCLK / 2*Fsck - 1
	SPI1BRG = (24000000 / (2 * clkSpeed)) -1;	// If called with too low clk speed, it will underflow to 511 == slowest
	
	// Clear status register
	SPI1STAT = 0;
	
	// Clear other config register, more for audio
	SPI1CON2 = 0;	// None used, only for audio etc.
	
	// Bulk of things
	SPI1CONbits.FRMEN = 0;
	SPI1CONbits.FRMSYNC = 0;
	SPI1CONbits.FRMPOL = 0;
	SPI1CONbits.MSSEN = 0;		// SS pin driven manually
	SPI1CONbits.FRMSYPW = 0;
	SPI1CONbits.FRMCNT = 0;
	SPI1CONbits.MCLKSEL = 0;	// PBCLK is used by the BRG
	SPI1CONbits.SPIFE = 0;
	SPI1CONbits.ENHBUF = 0;		// Don't need FIFOs and things
	SPI1CONbits.SIDL = 0;		// Continue module operation in idle mode
	SPI1CONbits.DISSDO = 0;		// SDOx pin used by module
	SPI1CONbits.MODE32 = 0;		// 8-bit transmissions
	SPI1CONbits.MODE16 = 0;		// 8-bit transmissions
	SPI1CONbits.SMP = sample;	// Input data sampled at middle of data output time
	SPI1CONbits.CKE = edge;		// Data changes on: 0 == idle to active, 1 == active to idle clock
	SPI1CONbits.SSEN = 0;		// SSx pin not used for Slave mode, controlled by port function
	SPI1CONbits.CKP = polarity;	// Clock polarity, 0 == idle low, 1 == idle high
	SPI1CONbits.MSTEN = 1;		// Master mode enabled
	SPI1CONbits.DISSDI = 0;		// SDI pin controlled by SPI module
	SPI1CONbits.STXISEL = 0;	// TX interrupt, not used...
	SPI1CONbits.SRXISEL = 0;	// RX interrupt, not used...
	
	SPI1CONbits.ON = 1;	// At the end, turn on
}

void SPIDrv_SendBlocking(uint8_t * buffer, uint32_t length){

	uint32_t counter = 0;
	volatile uint32_t discard;
	
	for (counter = 0; counter<length; counter++){
		SPI1BUF = buffer[counter];
		while(SPI1STATbits.SPIRBF == 0){}
		discard = SPI1BUF;
	}
	
}

void SPIDrv_SendReceiveBlocking(uint8_t * bufferIn, uint8_t * bufferOut, uint32_t length){

	uint32_t counter = 0;
	volatile uint32_t discard;

	for (counter = 0; counter<length; counter++){
		SPI1BUF = bufferIn[counter];
		while(SPI1STATbits.SPIRBF == 0){}
		bufferOut[counter] = SPI1BUF;
	}

}


