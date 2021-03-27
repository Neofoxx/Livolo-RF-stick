#include <p32xxxx.h>
#include <UARTDrv.h>
#include <GPIODrv.h>
#include <system.h>
#include <inttypes.h>
#include <interrupt.h>

// Init UART - UART2
// RX is RC8 aka. RPC8. Page 132 of manual says to set U2RXR<3:0> to ?
// TX is RB3 aka. RPB3. Page 134 says to set RP*** to ?
// Don't forget to set TRIS accordingly
// Set RX as pull-up - won't hurt. CNPUx register.
// Pin On-change notifications need to be disabled (default)
// Fun fact, all I/O modules have CLR, SET and INV register for atomic bit-manipulation. Neat

volatile uint32_t count = 0;
uint8_t temp;

// For interrupts http://wiki.pinguino.cc/index.php/PIC32MX_Development#Using_CHANGE_NOTICE_interrupt
//U1RX – UART1 Receive Done IRQ40 Vector32 IFS1<8> IEC1<8> IPC8<4:2> IPC8<1:0>
//U1TX – UART1 Transfer Done IRQ41 Vector32 IFS1<9> IEC1<9> IPC8<4:2> IPC8<1:0>

// As defined in interrupt.h/c
INTERRUPT(UART2Interrupt){
	// Should check if TX or RX interrupt
	count++;		// Increase counter, for our test
	temp = U2RXREG;	// Readout data, otherwise we'll be stuck here
	IFS1bits.U2RXIF = 0;	// Clear bits. TODO nicer.
}


void UARTDrv_Init(uint32_t baud){
	U2MODEbits.ON = 0;

	UART_TX_TRISbits.UART_TX_TRISPIN = 0;	// 0 == output
	UART_TX_LATbits.UART_TX_LATPIN = 1;		// Set high, as UART is Idle High
	UART_TX_RP_REG = UART_TX_RP_VAL;		// Remap to proper pin

	UART_RX_TRISbits.UART_TX_TRISPIN = 1;						// 1 == input
	UART_RX_PULLREG = UART_RX_PULLREG | UART_RX_PULLBIT;	// Enable pull-up
	U2RXR = UART_RX_REMAP_VAL;									// Set to which pin

	U2MODEbits.SIDL = 0;	// Stop when in IDLE mode
	U2MODEbits.IREN	= 0;	// Disable IrDA
	U2MODEbits.RTSMD = 0;	// Don't care, RTS not used
	U2MODEbits.UEN = 0;		// TX & RX controlled by UART peripheral, RTC & CTS are not.
	U2MODEbits.WAKE = 0;	// Don't wake up from sleep
	U2MODEbits.LPBACK = 0;	// Loopback mode disabled
	U2MODEbits.ABAUD = 0;	// No autobauding
	U2MODEbits.RXINV = 0;	// Idle HIGH
	U2MODEbits.BRGH = 0;	// Standard speed mode - 16x baud clock
	U2MODEbits.PDSEL = 0;	// 8 bits, no parity
	U2MODEbits.STSEL = 0;	// 1 stop bit

	U2STAbits.ADM_EN = 0;	// Don't care for auto address detection, unused
	U2STAbits.ADDR = 0;		// Don't care for auto address mark
	U2STAbits.UTXISEL = 00;	// Generate interrupt, when at least one space available (unused)
	U2STAbits.UTXINV = 0;	// Idle HIGH
	U2STAbits.URXEN = 1;	// UART receiver pin enabled
	U2STAbits.UTXBRK = 0;	// Don't send breaks.
	U2STAbits.UTXEN = 1;	// Uart transmitter pin enabled
	U2STAbits.URXISEL = 0;	// Interrupt what receiver buffer not empty
	U2STAbits.ADDEN = 0;	// Address detect mode disabled (unused)
	U2STAbits.OERR = 0;		// Clear RX Overrun bit - not important at this point

	// (PBCLK/BRGH?4:16)/BAUD - 1
	U2BRG = (GetPeripheralClock() / (U2MODEbits.BRGH ? 4 : 16)) / baud - 1;

	// New - setup interrupt
	IPC9bits.U2IP = 1;		// Priorty = 1 (one above disabled)
	IPC9bits.U2IS = 0;		// Subpriority = 0 (least)
	IEC1bits.U2RXIE = 1;	// Enable interrupt

	U2MODEbits.ON = 1;
}

void UARTDrv_SendBlocking(uint8_t * buffer, uint32_t length){

	uint32_t counter = 0;

	for (counter = 0; counter<length; counter++){
		while(U2STAbits.UTXBF){ asm("nop"); }
		U2TXREG = buffer[counter];
	}

	// Wait until sent
	while(U2STAbits.TRMT){
		_nop();
	}
}

uint32_t UARTDrv_GetCount(){
	return count;
}

