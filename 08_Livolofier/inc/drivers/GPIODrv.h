#ifndef GPIODRV_H_573f2ca8b43e44209acc90f0f9b7de5d
#define GPIODRV_H_573f2ca8b43e44209acc90f0f9b7de5d

// LED - RED
#define LEDRED_TRISbits		TRISBbits
#define LEDRED_TRISPIN		TRISB4
#define LEDRED_LATbits		LATBbits
#define LEDRED_LATPIN		LATB4
#define LEDRED_LATINV		LATBINV
#define LEDRED_MASK			(1<<4)

// LED - GREEN
#define LEDGREEN_TRISbits	TRISAbits
#define LEDGREEN_TRISPIN	TRISA4
#define LEDGREEN_LATbits	LATAbits
#define LEDGREEN_LATPIN		LATA4
#define LEDGREEN_LATINV		LATAINV
#define LEDGREEN_MASK		(1<<4)



// Button
#define BTN_TRISbits		TRISAbits
#define BTN_TRISPIN			TRISA3
#define BTN_PORTbits		PORTAbits
#define BTN_PORTPIN			RA3
#define BTN_PULLREG			CNPUA	// Call also pull down with CNPDx
#define BTN_PULLBIT			(1<<3)
#define BTN_ANSELbits		ANSELAbits
#define BTN_ANSELPIN		ANSA3



// UART - UART2, need to remap
#define UART_TX_TRISbits		TRISBbits
#define UART_TX_TRISPIN			TRISB12
#define UART_TX_LATbits			LATBbits
#define UART_TX_LATPIN			LATB12
#define UART_TX_REMAP_BITS		RPOR2bits
#define UART_TX_REMAP_SUB		RP12R
#define UART_TX_REMAP_VAL		1	// Remap U2TX to that pin
#define UART_TX_ANSELbits		ANSELBbits
#define UART_TX_ANSELPIN		ANSB12

#define UART_RX_TRISbits		TRISBbits
#define UART_RX_TRISPIN			TRISB13
#define UART_RX_PULLREG			CNPUB	// UARTs are idle high, so pull-up
#define UART_RX_PULLBIT			(1<<13)
#define UART_RX_REMAP_BITS		RPINR9bits
#define UART_RX_REMAP_SUB		U2RXR
#define UART_RX_REMAP_VAL		13	// Remap pin 13 to UART2 RX
#define UART_RX_ANSELbits		ANSELBbits
#define UART_RX_ANSELPIN		ANSB13



#define UART_MODE_bits		U2MODEbits
#define UART_STA_bits		U2STAbits
#define UART_BRG_reg		U2BRG
#define UART_TX_reg			U2TXREG
#define UART_RX_reg			U2RXREG




// SPI - SPI2
// SDI - MISO
#define SPI_SDI_TRISbits		TRISBbits
#define SPI_SDI_TRISPIN			TRISB14
#define SPI_SDI_PULLREG			CNPDB	// Prevent floating pins, pull-up or pull-down is OK.
#define SPI_SDI_REMAP_BITS		RPINR11bits
#define SPI_SDI_REMAP_SUB		SDI2R
#define SPI_SDI_REMAP_VAL		9	// Remap RB14, RP9 onto SDI
#define SPI_SDI_MASK			(1<<14)
#define SPI_SDI_ANSELbits		ANSELBbits
#define SPI_SDI_ANSELPIN		ANSB14

// SDO - MOSI
#define SPI_SDO_TRISbits		TRISBbits
#define SPI_SDO_TRISPIN			TRISB9
#define SPI_SDO_REMAP_BITS		RPOR1bits
#define SPI_SDO_REMAP_SUB		RP8R	// RB9, RP8
#define SPI_SDO_REMAP_VAL		3	// Remap SDO2 to that pin

// SCK
#define SPI_SCK_TRISbits		TRISBbits
#define SPI_SCK_TRISPIN			TRISB8
#define SPI_SCK_LATbits			LATBbits
#define SPI_SCK_LATPIN			LATB8
#define SPI_SCK_LATSET			LATBSET
#define SPI_SCK_LATCLR			LATBCLR
#define SPI_SCK_MASK			(1<<8)
#define SPI_SCK_REMAP_BITS		RPOR1bits
#define SPI_SCK_REMAP_SUB		RP7R
#define SPI_SCK_REMAP_VAL		4	// Remap SCK2 to that pin

// NSS - RFM69, active LOW, SW controlled
#define SPI_NSS_TRISbits		TRISBbits
#define SPI_NSS_TRISPIN			TRISB15
#define SPI_NSS_LATbits			LATBbits
#define SPI_NSS_LATPIN			LATB15
#define SPI_NSS_LATSET			LATBSET
#define SPI_NSS_LATCLR			LATBCLR
#define SPI_NSS_MASK			(1<<15)
#define SPI_NSS_ANSELbits		ANSELBbits
#define SPI_NSS_ANSELPIN		ANSB15

// DIO2 SDI&SDO dual-action pin
#define SPI_DIO2_TRISbits			TRISBbits
#define SPI_DIO2_TRISPIN			TRISB7
#define SPI_DIO2_LATbits			LATBbits
#define SPI_DIO2_LATPIN				LATB7
#define SPI_DIO2_DI_REMAP_BITS		RPINR11bits
#define SPI_DIO2_DI_REMAP_SUB		SDI2R
#define SPI_DIO2_DI_REMAP_VAL		11	// Remap RP11 onto SDI
#define SPI_DIO2_DO_REMAP_BITS		RPOR2bits
#define SPI_DIO2_DO_REMAP_SUB		RP11R
#define SPI_DIO2_DO_REMAP_VAL		3	// Remap SDO2 to that pin



// RFM69
// RESET - active high
#define RFM_RESET_TRISbits		TRISBbits
#define RFM_RESET_TRISPIN		TRISB2
#define RFM_RESET_LATbits		LATBbits
#define RFM_RESET_LATPIN		LATB2
#define RFM_RESET_LATSET		LATBSET
#define RFM_RESET_LATCLR		LATBCLR
#define RFM_RESET_MASK			(1<<2)
#define RFM_RESET_ANSELbits		ANSELBbits
#define RFM_RESET_ANSELPIN		ANSB2


#endif
