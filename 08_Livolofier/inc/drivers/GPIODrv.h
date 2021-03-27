#ifndef GPIODRV_H_573f2ca8b43e44209acc90f0f9b7de5d
#define GPIODRV_H_573f2ca8b43e44209acc90f0f9b7de5d

// LED
#define LEDUSER_TRISbits		TRISCbits
#define LEDUSER_TRISPIN			TRISC4
#define LEDUSER_LATbits			LATCbits
#define LEDUSER_LATPIN			LATC4
#define LEDUSER_LATINV			LATCINV
#define LEDUSER_MASK			(1<<4)

// Button
#define BTNUSER_TRISbits		TRISCbits
#define BTNUSER_TRISPIN			TRISC5
#define BTNUSER_PORTbits		PORTCbits
#define BTNUSER_PORTPIN			RC5
#define BTNUSER_PULLREG			CNPUC	// Call also pull down with CNPDx
#define BTNUSER_PULLBIT			(1<<5)

// UART
#define UART_TX_TRISbits		TRISCbits
#define UART_TX_TRISPIN			TRISC8
#define UART_TX_LATbits			LATCbits
#define UART_TX_LATPIN			LATC8
#define UART_TX_RP_REG			RPC9R
#define UART_TX_RP_VAL			0b0010	// U2TX

#define UART_RX_TRISbits		TRISCbits
#define UART_RX_TRISPIN			TRISC9
#define UART_RX_PULLREG			CNPUC	// UARTs are idle high, so pull-up
#define UART_RX_PULLBIT			(1<<9)
#define UART_RX_REMAP_VAL		0b0110

// SPI - SPI1
// SDI - MISO
#define SPI_SDI_TRISbits		TRISBbits
#define SPI_SDI_TRISPIN			TRISB5
#define SPI_SDI_PULLREG			CNPDB	// Prevent floating pins, pull-up or pull-down is OK.
#define SPI_SDI_REMAP_REG		SDI1R
#define SPI_SDI_REMAP_VAL		0b0001	// RPB5
#define SPI_SDI_MASK			(1<<5)
// SDO - MOSI
#define SPI_SDO_TRISbits		TRISBbits
#define SPI_SDO_TRISPIN			TRISB13
#define SPI_SDO_RP_REG			RPB13R
#define SPI_SDO_RP_VAL			0b0011	// SDO1
// SCK
#define SPI_SCK_TRISbits		TRISBbits
#define SPI_SCK_TRISPIN			TRISB14
#define SPI_SCK_LATbits			LATBbits
#define SPI_SCK_LATPIN			LATB14
#define SPI_SCK_LATSET			LATBSET
#define SPI_SCK_LATCLR			LATBCLR
#define SPI_SCK_MASK			(1<<14)
// NSS - RFM69, active LOW
#define SPI_NSS_TRISbits		TRISCbits
#define SPI_NSS_TRISPIN			TRISC7
#define SPI_NSS_LATbits			LATCbits
#define SPI_NSS_LATPIN			LATC7
#define SPI_NSS_LATSET			LATCSET
#define SPI_NSS_LATCLR			LATCCLR
#define SPI_NSS_MASK			(1<<7)
// DIO2 SDI&SDO dual-action pin
#define SPI_DIO2_TRISbits		TRISBbits
#define SPI_DIO2_TRISPIN		TRISB1
#define SPI_DIO2_ANSELbits		ANSELBbits
#define SPI_DIO2_ANSELPIN		ANSB1
#define SPI_DIO2_LATbits		LATBbits
#define SPI_DIO2_LATPIN			LATB1
#define SPI_DIO2_REMAP_REG		SDI1R
#define SPI_DIO2_REMAP_VAL		0b0010	// RPB1
#define SPI_DIO2_MASK			(1<<1)
#define SPI_DIO2_RP_REG			RPB1R
#define SPI_DIO2_RP_VAL			0b0011	// SDO1


// RFM69
// RESET - active high
#define RFM_RESET_TRISbits		TRISCbits
#define RFM_RESET_TRISPIN		TRISC6
#define RFM_RESET_LATbits		LATCbits
#define RFM_RESET_LATPIN		LATC6
#define RFM_RESET_LATSET		LATCSET
#define RFM_RESET_LATCLR		LATCCLR
#define RFM_RESET_MASK			(1<<6)



#endif
