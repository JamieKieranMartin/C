// Include the AVR IO library
#include <avr/io.h>

// Include the CPU Speed information
#include "cpu_speed.h"
#include "macros.h"

void setup() {
	set_clock_speed(CPU_8MHz);

	// Enable left LED for output
	SET_BIT(DDRB, 2);

	// Turn the LED on
	SET_BIT(PORTB, 2);
}

void process() {
	// Do nothing this time.
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
	}
}