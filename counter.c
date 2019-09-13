
/*
 *   File: counter.c
 *   Description: C program for the ATMEL AVR microcontroller (ATmega32 chip)
 *   Tries to emulate a countdown timer. 
 *    ** NOTE THIS PROGRAM DOES NOT USE TIMERS, THERFERE IS NOT REAL TIME **
 *
 *
 *  Includes (pretty much compulsory for using the Teensy this semester)
 * 	- avr/io.h: port and pin definitions (i.e. DDRB, PORTB, PB1, etc)
 *	- util/delay.h: '_delay_ms()' function
 *	- cpu_speed.h: macros for forcing the CPU speed to 8MHz (nothing else should ever be used!)
 *  - graphics.h: library to interact with the LCD
 *  - stdio.h: used to include functions for std input and std output.
 */


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

#include "graphics.h"
#include "cpu_speed.h"
#include "macros.h"


//Global variable for the clock system
 int   clock_millisecond=0;

 int        clock_second=0;
 int        clock_minute=0;
 int        clock_hour=0;


void setup() {

    // Set the CPU speed to 8MHz (you must also be compiling at 8MHz)
	set_clock_speed(CPU_8MHz);
	
	//initialise the LCD screen
    lcd_init(LCD_DEFAULT_CONTRAST);
    
	// Input from the left and right buttons
	CLEAR_BIT(DDRF, 5);
	CLEAR_BIT(DDRF, 6);
}


//write the main file
int main(void) {

     //Initialise buttons
     setup();

    //counter for the number of loops
    int counter = 0;
    
    //used to store characters to draw on the screen
    char buffer[32];
       
    //used to increase/decrease the speed of the counter
    int prescaler = 40;
    
    //auxliary variables to draw a line.
    int l = 10;
    int i = 0;
    
    
    while (1){
    
    //clear any characters that were written on the screen
   clear_screen();
    
    //fill a buffer (array) of charactes with the string "  ", in the position x,y
    draw_string(4,0, "Welcome CAB202", FG_COLOUR);
    draw_string(12,9,"Timer Demo!",FG_COLOUR);
    
            
        //Read from Port F to check input - SW3
        //If the correct pin connected to the button is a 1
        //then increment prescaler. makes it run faster
        if(BIT_IS_SET(PINF,5)){
            prescaler--;// = prescaler - 1;            
        }
        
        //avoid this variable from being zero
        if (prescaler < 1) {
            prescaler = 40;
        }
        
        // Read from Port F to check input - SW1
        // If the correct pin connected to the button is a 1
        // Then reset counter
        if(BIT_IS_SET(PINF,6)){
            counter=clock_minute=clock_second=clock_hour=0;
            prescaler=40;           
        }
        
        
       //increase seconds every time counter increases by 1
       if (counter%prescaler == (prescaler-1)){
            clock_second++;
        }
        
        //once seconds are 59, increase minutes
        if (clock_second > 59) {
            clock_second=0;
            clock_minute++;
        }
        
        //once minutes are 59, increase hours
        if (clock_minute > 59) {
            clock_minute=0;
            clock_hour++;
        }
    
        //draw the characters on the screen
        draw_string(20,40,buffer,FG_COLOUR);
              
        // Draw a fancy line wheel...
         draw_line(42, 27, round((double) 42+l*cos(((double) i)/8*M_PI)),round((double) 27+l*sin(((double) i)/8*M_PI)),FG_COLOUR);
               
        //fill buffer with characters
        sprintf(buffer, "%2.2d:%2.2d:%2.2d", clock_hour, clock_minute, clock_second);
       
        //refresh pixels
        show_screen();
        
        //increase loop counter
     counter++;
     //increase angle for line wheel
     i++;
        
    }
        
   
	return 0;
}



