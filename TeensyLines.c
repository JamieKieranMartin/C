#include <stdint.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>

#include "lcd_model.h"



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
	show_screen();
}

void process(void) {

	static double fraction = 0.0;
	clear_screen();

	int x1 = LCD_X * fraction; 
	int x2 = LCD_X - x1;
	int y1 = LCD_Y * fraction;
	int y2 = LCD_Y - y1;

	fraction += 0.01;

	if ( fraction >= 0.5 ) fraction = 0.0;
	

	draw_line(x1, y1, x2, y1, BG_COLOUR);
	draw_line(x1, y2, x2, y2, BG_COLOUR);
	draw_line(x1, y1, x1, y2, BG_COLOUR);
	draw_line(x2, y1, x2, y2, BG_COLOUR);
	

	show_screen();
}

int main(void) {

	setup();

	for ( ;; ) {
		process();
		_delay_ms(100);
		
	}
}
