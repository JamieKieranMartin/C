#include <avr/io.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <macros.h>

void setup_game_controller( void ) { 
    //  (a) Set the CPU speed to 8MHz (you must also be compiling at 8MHz).

    //  (b) Configure the data direction register for Port B to enable output
    //  to LED0 and LED1. The data direction for LED0 is controlled by Pin 2, 
    //  while that for LED1 is controlled by Pin 3. No other pins should be 
    //  affected.

    //  (c) Configure the data direction register for Port B to enable input
    //  from the Centre, Left, and Down switches on the joystick. These 
    //  are controlled by Pins 0, 1, and 7 respectively. No other pins should be 
    //  affected.

    //  (d) Turn off LED0, LED1, and all other outputs connected to Port B, by 
    //  clearing all bits in the Port B output register.

    //  (e) Configure the data direction register for Port D to enable input
    //  from the Right and Up switches on the joystick. These are controlled by
    //  Pins 0 and 1 respectively. No other pins should be affected by this 
    //  operation.
}

void process_game_controller( void ) {
    //  (f) Test the relevant pin in the Port D input register to determine
    //  if the Up switch is closed. If it is, turn on both LEDs without 
    //  disrupting any other pins.

    //  (g) Test the relevant pin of the Port B input register
    //  to determine if the Down switch is closed. If it is, turn off both LEDs
    //  without affecting any other pins.

    //  (h) Test the relevant pin of the Port B input register
    //  to determine if the Left switch is closed. If it is, turn on the left LED
    //  and turn off the right LED without affecting any other pins.

    //  (i) Test the relevant pin of the Port D input register 
    //	to determine if the Right switch is closed. If it is, turn on the right LED
    //  and turn off the left LED without affecting any other pins.

    //  (j) Test the relevant pin of the Port B input register
    //  to determine if the Centre switch is closed. If it is, toggle both LEDs
    //  without affecting any other pins.
}


int main(void) {
    setup_game_controller();

    while ( 1 ) {
        process_game_controller();
        _delay_ms(100);
    }

    return 0;
}
