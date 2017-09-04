#include <msp430.h>
#include "spi.h"

void InitializeSPI()
{	

	SET_USCIA0_MISO_AS_AN_INPUT;
	SET_USCIA0_MOSI_AS_AN_OUTPUT;

	UCA0CTL1 = UCSWRST; 		// Set up the SPI mode

	P1SEL  = BIT1|BIT2|BIT4;	// Set pins 1, 2, and 4 to comm mode
	P1SEL2 = BIT1|BIT2|BIT4;

	// Mode 0, MSB first, 8-bit SPI master, three-pin mode
	UCA0CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC;

	UCA0CTL1 |= UCSSEL_2;		// Use SMCLK, keep SW reset

	UCA0BR0 |= 0x02;			// Divide provided (SMCLK) clock by 2
	UCA0BR1 |= 0x00;			// For 16MHz, this should divide to 8Mhz

	UCA0MCTL = 0; 				// No modulation

	UCA0CTL1 &= ~UCSWRST; 		// Clear SW reset, resume operation

}

void SPISendByte(unsigned char SendValue)
{

	UCA0TXBUF = SendValue; 			// Write to the transaction buffer
	while(UCA0STAT&BIT0); 			// wait while busy bit is high

}

unsigned char SPIReceiveByte()
{

	UCA0TXBUF = 0x00;				// Send dummy to transmit buffer
	while(UCA0STAT&BIT0); 			// wait while busy bit is high
	return UCA0RXBUF;				// Return receive buffer

}

void SPISendAddress(unsigned long Address) {
	SPISendByte((unsigned char)(Address>>16));
	SPISendByte((unsigned char)(Address>>8));
	SPISendByte((unsigned char)(Address));
}
