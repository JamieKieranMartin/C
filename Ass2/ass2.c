#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"

//	A volatile global variable called state_count.
uint8_t state_count[7] = { 0 };
//	A volatile global 8-bit unsigned global variable 
//	called is_pressed which will store the current state of the switch.
uint8_t is_pressed[7];
void update_history(uint8_t pin, int pin_loc, int i) {
    uint8_t mask = 0b00000111;
    state_count[i] = (( state_count[i] << 1) & mask) | BIT_IS_SET(pin, pin_loc);
    if ( state_count[i] == mask ) is_pressed[i] = 1;
    else if ( state_count[i] == 0 ) is_pressed[i] = 0;
}

bool isPressed(int type) {
    static uint8_t prevState = 0;
    if ( is_pressed[type] != prevState ) return true;
    else return false;
}

void start_screen() {
    draw_string(5, 10, "Jamie Martin", FG_COLOUR);
    draw_string(5, 20, "n10212361", FG_COLOUR);
    draw_string(5, 30, "Thomas + Jerrence", FG_COLOUR);

    show_screen();
    while(1) { if (isPressed(5)) { lcd_clear(); break; } }
}

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	lcd_clear();
	//	Initialise Timer 0 in normal mode so that it overflows 
	//	with a period of approximately 0.008 seconds.
    TCCR0A = 0;
    TCCR0B = 4;
	//	Enable timer overflow interrupt for Timer 0.
    TIMSK0 = 1;
	//	Turn on interrupts.
    sei();
    // data direction register / output
    SET_BIT(DDRB, 2); //LED0
    SET_BIT(DDRB, 3); // LED1 
    // PORTB = 0b00000000; // Clear all outputs on PORTB
    // Enable left + right buttons for data direction register / input
    CLEAR_BIT(DDRF, 6); // LEFT
    CLEAR_BIT(DDRF, 5); // RIGHT
	//  Enable the joystick switch for data direction register / input
    CLEAR_BIT(DDRB, 0); // CENTER
    CLEAR_BIT(DDRB, 1); // LEFT
    CLEAR_BIT(DDRB, 7); // DOWN
    CLEAR_BIT(DDRD, 0); // RIGHT
    CLEAR_BIT(DDRD, 1); // UP
    // show screen and display start;
	show_screen();
    start_screen();
}

//	An interrupt service routine to process timer overflow interrupts for Timer 0
ISR(TIMER0_OVF_vect) {
    // JOYSTICK UP
    update_history(PIND, 1, 0);
    // JOYSTICK DOWN
    update_history(PINB, 7, 1);
    // JOYSTICK LEFT
    update_history(PINB, 1, 2);
    // JOYSTICK RIGHT
    update_history(PIND, 0, 3);
    // JOYSTICK CENTRE
    update_history(PINB, 0, 4);
    // RIGHT BUTTON
    update_history(PINF, 5, 5);
    // LEFT BUTTON
    update_history(PINF, 6, 6);
}

void process(void) {
    // JOYSTICK UP
    if ( isPressed(0) ) {
   
    }
    // JOYSTICK DOWN
    if ( isPressed(1) ) {

    }
    // JOYSTICK LEFT
    if ( isPressed(2) ) {
  
    }
    // JOYSTICK RIGHT
    if ( isPressed(3) ) {

    }
    // JOYSTICK CENTRE
    if ( isPressed(4) ) {

    }
    // RIGHT BUTTON
    if ( isPressed(5) ) {

    }
    // LEFT BUTTON
    if ( isPressed(6) ) {
   
    }
}

int main(void) {
	setup();
    start_screen();
	for ( ;; ) {
        //process_game_controller();
		process();
	}
}
