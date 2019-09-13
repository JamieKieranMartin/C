#include <stdint.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <stdio.h>
#include <stdlib.h>

#include <macros.h>
#include <lcd_model.h>
#include "lcd.h"



/*
**  Initialise the LCD display.
*/
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


uint8_t cactus_original[8] = {
    0b10010001,
    0b10010001,
    0b10100010,
    0b10010100,
    0b01001100,
    0b00110000,
    0b00010000,
    0b00010000,
};
uint8_t cactus_direct[8];
uint8_t x, y;


/*
**  Convert cactus into vertical slices for direct drawing.
**  This will need to be amended if cactus is larger than 8x8.
*/

void setup_cactus(void) {
    // Visit each column of output bitmap
    for (int i = 0; i < 8; i++) {
        // Visit each row of output bitmap
        for (int j = 0; j < 8; j++) {
            // Kind of like: cactus_direct[i][j] = cactus_original[j][7-i].
            // Flip about the major diagonal.
            uint8_t bit_val = BIT_VALUE(cactus_original[j], (7 - i));
            WRITE_BIT(cactus_direct[i], j, bit_val);
        }
    }

}



/*
**  Draw cactus face directly to LCD.
**  (Notice: y-coordinate.)
*/
void draw_data(int x, int y) {
    LCD_CMD(lcd_set_function, lcd_instr_basic | lcd_addr_horizontal);
    LCD_CMD(lcd_set_x_addr, x);
    LCD_CMD(lcd_set_y_addr, y / 8);

    for (int i = 0; i < 8; i++) {
        LCD_DATA(cactus_direct[i]);
    }
}


/*
**	Remove cactus from LCD.
*/

void erase_data(void) {
    LCD_CMD(lcd_set_function, lcd_instr_basic | lcd_addr_horizontal);
    LCD_CMD(lcd_set_x_addr, 0);
    LCD_CMD(lcd_set_y_addr, 0 / 8);

    for (int i = 0; i < LCD_X*LCD_Y/8; i++) {
        LCD_DATA(0);
    }
}


void setup(void) {
    set_clock_speed(CPU_8MHz);
    new_lcd_init(LCD_DEFAULT_CONTRAST);
    erase_data();
    setup_cactus();
}

void process(void) {
    erase_data();
    
    x = (x + 1) % (LCD_X - 8);
    
    draw_data(x,10);
    draw_data(x+5,24);
    
}

int main(void) {
    setup();

    for (;; ) {
        process();
        _delay_ms(250);
    }
}
