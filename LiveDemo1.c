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
#include <ascii_font.h>
#include <graphics.h>

#define SQRT(x,y) sqrt(x*x + y*y)

int dx1,dy1,x1,x2,y1,y2;
int dxa1,dya1,xa1,xa2,ya1,ya2;
int dxb1,dyb1,xb1,xb2,yb1,yb2;
int xc1,xc2,yc1,yc2;
double IsCol;
char buffer[32];
int xc,yc;


void setup(void) {
    set_clock_speed(CPU_8MHz);
    lcd_init(LCD_DEFAULT_CONTRAST);
    clear_screen();
    show_screen();
    
     // Choose any (x,y) line 1
    x1 = 5; x2 = 15;
    y1 = 5; y2 = 15;
 
    // Choose any (x,y) line 2
    xa1 = 40; xa2 = 20;
    ya1 = 20; ya2 = 30;
 
    // Choose any (x,y) line 3
    xb1 = 45; xb2 = 50;
    yb1 = 30; yb2 = 35;
    
    //pixel location
    xc=20;
    yc=12;
    
    //SET PORTB pin2 as output
    SET_BIT(DDRB, 2);
    
}

double CalcDistanceBetween2Points(int x1, int y1, int x2, int y2)
{
    return SQRT((x1-x2), (y1-y2));
}

double PointLinesOnLine (int x, int y, int x1, int y1, int x2, int y2, double allowedDistanceDifference)
{
    double dist1 = CalcDistanceBetween2Points(x, y, x1, y1);
    double dist2 = CalcDistanceBetween2Points(x, y, x2, y2);
    double dist3 = CalcDistanceBetween2Points(x1, y1, x2, y2);
    
    return abs(dist3 - (dist1 + dist2)) <= allowedDistanceDifference;
}


void process(void) {
    clear_screen();
    
    
    /////////////////////////////////
    dx1 = (dx1 + 1) % (LCD_X - 8); if ((x1+dx1) > 20 ) dx1=0;
    dy1 = (dy1 - 1) % (LCD_Y - 8); if ((y2+dy1) < 20 ) dy1=0;
    
    draw_line(x1+dx1,y1+dy1,x2+dx1,y2+dy1,BG_COLOUR);
    
    //draw a pixel at (xc,yc)
    draw_pixel(xc,yc,FG_COLOUR);
    //detect collision between pixel and line. note this method is depended 
    //on the tolerance. 
    IsCol = PointLinesOnLine (xc, yc, x1+dx1, y1+dy1, x2+dx1, y2+dy1, 10e-5);
    
    //draw state of IsCol on LCD
    sprintf(buffer, "%1.2f", IsCol);
    draw_string(40,40,buffer,FG_COLOUR);
    //trigger an action. Turn LED ON
    if (IsCol){
      SET_BIT(PORTB, 2);      
    }else{
      CLEAR_BIT(PORTB, 2);
    }
    
    //process is repeated for multiple lines and pixels
    
    /*
    ////////////////////////////////////
    dxa1 = (dxa1 + 1) % (LCD_X - 8);
    dya1 = (dya1 + 1) % (LCD_Y - 8);
    draw_line(xa1+dxa1,ya1+dya1,xa2+dxa1,ya2+dya1,BG_COLOUR);
    ////////////////////////////////////
    dxb1 = (dxb1 - 1) % (LCD_X - 8);
    dyb1 = (dyb1 + 1) % (LCD_Y - 8);
    draw_line(xb1+dxb1,yb1+dyb1,xb2+dxb1,yb2+dyb1,BG_COLOUR);
    */
    
    show_screen();
}

int main(void) {
    setup();

    for (;; ) {
        process();
        _delay_ms(250);
    }
}
