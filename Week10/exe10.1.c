#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"

//	(a) Implement a function called uart_initialise which requires no arguments and
//	returns no value.
void uart_initialise() {
	//	(a.a) Set the data transmission rate to 9600 bits per second,
	//	using the formula provided in the Topic 10 Technical Notes.
    UBRR1 = (F_CPU / 8 / 9600 - 1) / 2;
	//	(a.b) Set the UART speed to normal speed.
    UCSR1A = (0<<U2X1);
	//	(a.c) Enable receive and transmit, but do not enable any UART-related
	//	interrupts.
    UCSR1B = (1<<TXEN1) | (1<<RXEN1); 
	//	(a.d) Set the character size to 7 bits, with no parity
	//	and 1 stop bit.
    UCSR1C = (0<<UPM11) | (0<<UPM10) | (0<<USBS1) | (1<<UCSZ11) | (0<<UCSZ10);
//	(b) End of function uart_initialise
}

int main(void) {
	uart_initialise();

	for ( ;; ) {}
}
