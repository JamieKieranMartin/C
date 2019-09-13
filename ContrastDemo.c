#include <stdint.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <stdio.h>

#include <graphics.h>
#include <macros.h>

#include "lcd_model.h"


char buffer[10];


void new_lcd_init(uint8_t contrast) {
	// Set up the pins connected to the LCD as outputs
	SET_OUTPUT(DDRD, SCEPIN); // Chip select -- when low, tells LCD we're sending data
	SET_OUTPUT(DDRB, RSTPIN); // Chip Reset
	SET_OUTPUT(DDRB, DCPIN);  // Data / Command selector
	SET_OUTPUT(DDRB, DINPIN); // Data input to LCD
	SET_OUTPUT(DDRF, SCKPIN); // Clock input to LCD
	

	CLEAR_BIT(PORTB, RSTPIN); // Reset LCD
	SET_BIT(PORTD, SCEPIN);   // Tell LCD we're not sending data.
	SET_BIT(PORTB, RSTPIN);   // Stop resetting LCD

	LCD_CMD(lcd_set_function, lcd_instr_extended);
	LCD_CMD(lcd_set_contrast, contrast);
	LCD_CMD(lcd_set_temp_coeff, 0);
	LCD_CMD(lcd_set_bias, 3);

	LCD_CMD(lcd_set_function, lcd_instr_basic);
	LCD_CMD(lcd_set_display_mode, lcd_display_normal);
	LCD_CMD(lcd_set_x_addr, 0);
	LCD_CMD(lcd_set_y_addr, 0);
}

void setup(void) {
	set_clock_speed(CPU_8MHz);
	new_lcd_init(LCD_DEFAULT_CONTRAST);
	clear_screen();
	draw_string( 10, 10, "Hello Cab202!", FG_COLOUR );
	show_screen();
}



void draw_int(uint8_t x, uint8_t y, int t) {
	snprintf( buffer, 10, "%d", t );
	draw_string( x, y, buffer, FG_COLOUR );
}

void process(void) {
	static uint8_t contrast = 0;
	contrast ++;
	if ( contrast > 127 ) contrast = 0;
	
	draw_string(0, LCD_Y/2, "contrast=", FG_COLOUR );
	draw_int( LCD_X/2+5, LCD_Y/2, contrast );
	show_screen();
	
	LCD_CMD( lcd_set_function, lcd_instr_extended );
	LCD_CMD( lcd_set_contrast, contrast );
	LCD_CMD( lcd_set_function, lcd_instr_basic );
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
		_delay_ms(100);
	}
}
