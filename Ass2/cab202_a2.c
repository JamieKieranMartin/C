// CAB202 Semester 2 2019 | Assessment 2 - Tom & Jerry Teensy PewPew game
// By Trevor Waturuocha (N10209921) | Tutor: Emily Corser
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"

/*-------------Global Variables------------*/
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 // Pi value to randomise Tom's direction
#endif
#define s_h 9 // Status height (9 pixels)
#define map_size 50 // Maximum limit for map array
#define max_w 20     // Maximum limit for weapons count
#define max_t 5     // Maximum limit for traps count
#define max_c 5     // Maximum limit for cheese count
#define max_d 1     // Maximum limit for door count
#define THRESHOLD (1000) // Threshold for button press

// Buffer
char buffer[100]; // Array buffer to store characters for UI elements
// Main game
bool start_game = false; // Start game check
bool pause_game = false; // Pause game check
bool game_over = false; // Bool check if game over has been reached
char game_outcome;               // Char to check if the player has won or lost
// Timer
uint32_t overflow_counter = 0; // Timer overflow counter
int secs = 0;                  // Seconds counter
int mins = 0;                  // Minutes counter
// Status
int j_lives = 5; // Jerry player lives
int j_score = 4; // Jerry player score
// Levels
int level_count = 1;    // Count total levels loaded into the game
int level = 1; // Level counter
bool restart_level = false; // Bool check if game level has been reset
bool next_level = false;    // Bool check if new game level has been set
// Tom
double t_x = LCD_X - 5; // Tom x-position
double t_y = LCD_Y - 9; // Tom y-position
double t_sx = LCD_X - 5; // Tom x spawn position
double t_sy = LCD_Y - 9; // Tom y spawn position
double t_dx, t_dy; // Tom change in x and y position (speed)
const double step = 1.5;  // Setting Tom's optimal speed magnitude
double t_w = 3;       // Tom width
double t_h = 5;       // Tom height
// Jerry
double j_x = 0;       // Jerry x-position
double j_y = s_h + 1; // Jerry y-position
double j_sx = 0;       // Jerry x spawn position
double j_sy = s_h + 1; // Jerry y spawn position
double j_w = 3;       // Jerry width
double j_h = 5;       // Jerry height
bool j_hit = false; // Bool check if Jerry has been hit by mouse trap
bool j_hit2 = false;    // Bool check if Jerry has been hit by Tom
// Weapons
int weapons_total = 0; // Count total fireworks drawn on map
double weapon_x[max_w];      // Weapon x positions
double weapon_y[max_w];      // Weapon x positions
double weapon_dx, weapon_dy; // Weapon x/y speeds
bool weapon_fired;           // Bool check if weapon has been fired by Jerry
// Characters
double char_xdist; // X distance between characters
double char_ydist; // Y distance between characters
double char_dist;  // Overall distance between characters
// Cheese
double c_x[max_c];    // Cheese array of 5 X positions
double c_y[max_c];    // Cheese array of 5 Y positions
double c_w = 3;       // Cheese width
double c_h = 5;       // Cheese height
int total_c = 0; // Count total cheese on map
// Mousetraps
int total_traps = 0;   // Count total traps on map
double traps_x[max_t]; // Traps array of 5 X positions
double traps_y[max_t]; // Traps array of 5 Y positions
double traps_w = 3; // Trap width
double traps_h = 5; // Trap height
// Door
double door_x[max_d]; // Door array of 1 X position
double door_y[max_d]; // Door array of 1 Y position
double door_w = 3; // Door width
double door_h = 5; // Door height
int door_total = 0;   // Count total doors drawn on map
// Walls
double walls[map_size][4]; // Array to store wall values
// State machine for "button pressed"
bool pressed = false; // Bool check if button has been pressed
uint16_t closed_num = 0; // Counter for number of times that a switch is closed
uint16_t open_num = 0; // Counter for number of times that a switch is open


/*-------------Initialisation functions------------*/
// Setting up Teensy
void t_setup(void)
{
    // Initialising CPU
    set_clock_speed(CPU_8MHz);
    //	Enabling input from all switches and LEDs
    //	of the joystick.
    CLEAR_BIT(DDRB, 1); // LEFT STICK
    CLEAR_BIT(DDRD, 0); // RIGHT STICK
    CLEAR_BIT(DDRD, 1); // UP STICK
    CLEAR_BIT(DDRB, 7); // DOWN STICK
    CLEAR_BIT(DDRB, 0); // CENTRE STICK
    CLEAR_BIT(DDRF, 6); // LEFT BUTTON
    CLEAR_BIT(DDRF, 5); // RIGHT BUTTON
    SET_BIT(DDRB, 2);   // LED0
    SET_BIT(DDRB, 3);   // LED1
    SET_BIT(DDRD, 6);   // LED2
    //	Initialise the LCD display using the default contrast setting
    lcd_init(LCD_DEFAULT_CONTRAST);
}

/*-------------Game UI functions------------*/
// Start Screen
void draw_start()
{
    // If game has not started
    if (start_game == false)
    {
        // Draw student name, ID and game title
        draw_string((LCD_X / 2) - 42, 5, "Trevor Waturuocha", FG_COLOUR);
        draw_string((LCD_X / 2) - 22, 20, "n10209921", FG_COLOUR);
        draw_string((LCD_X / 2) - 35, 35, "Cat v.s. Mouse", FG_COLOUR);
    }
    // Click right button to start game if game has not started already
    if (BIT_IS_SET(PINF, 5) && start_game == false)
    {
        start_game = true;
    }
}

//	Interrupt service routine to process timer overflow interrupts for Timer 1.
ISR(TIMER1_OVF_vect)
{
    if (!pause_game){
        overflow_counter++;
        if(((overflow_counter * 65536 + TCNT1) * 256 / 8000000) >= 1){ // Elapsed time formula 
            secs++;
            overflow_counter = 0;
        }
        // If a minute has passed
        if (secs >= 60)
        {
            secs = 0;             // Reset seconds counter
            overflow_counter = 0; // Reset overflow counter
            mins++;
        }
        // For every two seconds, increase cheese count by 1
        if (secs % 2 == 0 && total_c < 5)
        {
            total_c++;
        }
        // For every three seconds, increase total traps count
            if (secs % 3 == 0 && total_traps < 5)
            {
                total_traps++;
            }
    }
}

//	Interrupt service routine to process switch debouncing with Timer 0.
ISR(TIMER0_OVF_vect)
{
    // Non-blocking debouncing switches
    // Detect a Click on left stick
    if (BIT_IS_SET(PINB, 1))
    {
        closed_num++; // Increment button closed counter
		open_num = 0; // Reset button open counter
		if ( closed_num  > THRESHOLD ) { // If counter exceeds threshold
			if ( !pressed ) { // If button has not been pressed
				closed_num = 0; // Reset button open counter
			}
			pressed = true; // Set button press state to true
		}
    }
    // Detect a Click on right stick
    if (BIT_IS_SET(PIND, 0))
    {
        closed_num++; // Increment button closed counter
		open_num = 0; // Reset button open counter
		if ( closed_num  > THRESHOLD ) { // If counter exceeds threshold
			if ( !pressed ) { // If button has not been pressed
				closed_num = 0; // Reset button open counter
			}
			pressed = true; // Set button press state to true
		}
    }
    // Detect a Click on up stick
    if (BIT_IS_SET(PIND, 1))
    {
        closed_num++; // Increment button closed counter
		open_num = 0; // Reset button open counter
		if ( closed_num  > THRESHOLD ) { // If counter exceeds threshold
			if ( !pressed ) { // If button has not been pressed
				closed_num = 0; // Reset button open counter
			}
			pressed = true; // Set button press state to true
		}
    }
    // Detect a Click on down stick
    if (BIT_IS_SET(PINB, 7))
    {
        closed_num++; // Increment button closed counter
		open_num = 0; // Reset button open counter
		if ( closed_num  > THRESHOLD ) { // If counter exceeds threshold
			if ( !pressed ) { // If button has not been pressed
				closed_num = 0; // Reset button open counter
			}
			pressed = true; // Set button press state to true
		}
    }
    // Detect a Click on left button
    if (BIT_IS_SET(PINF, 6))
    {
        closed_num++; // Increment button closed counter
		open_num = 0; // Reset button open counter
		if ( closed_num  > THRESHOLD ) { // If counter exceeds threshold
			if ( !pressed ) { // If button has not been pressed
				closed_num = 0; // Reset button open counter
			}
			pressed = true; // Set button press state to true
		}
    }
    // Detect a Click on right button
    if (BIT_IS_SET(PINF, 5))
    {
        closed_num++; // Increment button closed counter
		open_num = 0; // Reset button open counter
		if ( closed_num  > THRESHOLD ) { // If counter exceeds threshold
			if ( !pressed ) { // If button has not been pressed
				closed_num = 0; // Reset button closed counter
			}
			pressed = true; // Set button press state to true
		}
    }
    // If button has not been pressed
    else {
		open_num++; // Increment button open counter
		closed_num = 0; // Reset button closed counter
		if ( open_num > THRESHOLD ) { // If counter exceeds threshold
			if ( pressed ) { // If button has been pressed
				open_num = 0; // Reset button open counter
			}
			pressed = false; // Set button press state to false
		}
	}
}

// Status bar
void draw_status()
{
    int col = LCD_X / 5; // Variable for status column width
    // If game has started
    if (start_game == true)
    {
        snprintf(buffer, sizeof(buffer), "%d", level); // Formatting and storing level to buffer
        // Writing current level
        draw_string(0, 0, "L:", FG_COLOUR);
        draw_string(10, 0, buffer, FG_COLOUR);
        snprintf(buffer, sizeof(buffer), "%d", j_lives); // Formatting and storing Jerry lives to buffer
        // Writing Jerry's lives
        draw_string((col * 1) + 5, 0, "J:", FG_COLOUR);
        draw_string((col * 1) + 15, 0, buffer, FG_COLOUR);
        snprintf(buffer, sizeof(buffer), "%d", j_score); // Formatting and storing Jerry score to buffer
        // Writing Jerry's score
        draw_string((col * 2) + 5, 0, "S:", FG_COLOUR);
        draw_string((col * 2) + 15, 0, buffer, FG_COLOUR);
        snprintf(buffer, sizeof(buffer), "%02d", mins); // Formatting and storing minutes counter to buffer
        // Writing time elapsed in minutes
        draw_string((col * 3.5) + 3, 0, buffer, FG_COLOUR);
        draw_string((col * 3.5) + 13, 0, ":", FG_COLOUR);
        snprintf(buffer, sizeof(buffer), "%02d", secs); // Formatting and storing seconds counter to buffer
        // Writing time elapsed in minutes
        draw_string((col * 3.5) + 15, 0, buffer, FG_COLOUR);
        // Drawing border
        draw_line(0, 8, LCD_X, 8, FG_COLOUR);
    }
}

// Function to pause game
void pause()
{
    // Click right button to pause game if game has already started
    if (BIT_IS_SET(PINF, 5))
    {
        pause_game = !pause_game;
    }
}

// Function to restart game
void restart()
{
    // Click right button to restart game if player has reached game over screen
    if (BIT_IS_SET(PINF, 5) && game_over == true)
    {
        next_level = true;
        restart_level = true;
    }
}

// Function to fire a weapon
void fire()
{
    // Click centre button to fire weapon if player has scored 3 or more points
    if (BIT_IS_SET(PINB, 0) && j_score >= 3)
    {
        weapon_fired = true;
    }
}

// Function to check if two lines have collided together
bool lineLine(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
  // calculate the distance to intersection point
  double uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
  double uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
  // if uA and uB are between 0-1, lines are colliding
  if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
    return true;
  }
  return false;
}

// Function to check if a character object has collided with a wall
bool w_collided(double x, double y, double w, double h){
    // Looping through walls array
    for (int i = 0; i < map_size; i++)
    {
        // If there is a wall at either side of shape, return true, otherwise, return false
        bool left =   lineLine(walls[i][0], walls[i][1], walls[i][2], walls[i][3], x, y, x, y + h);
        bool right =  lineLine(walls[i][0], walls[i][1], walls[i][2], walls[i][3], x + w, y, x + w, y + h);
        bool top =    lineLine(walls[i][0], walls[i][1], walls[i][2], walls[i][3], x, y, x + w, y);
        bool bottom = lineLine(walls[i][0], walls[i][1], walls[i][2], walls[i][3], x, y + h, x + w, y + h);
        if (left || right || top || bottom) {
            return true;
        }
    }
    return false;
}

// Function to check if a character object has collided with another character object
bool c_collided(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2){
    if (x1 + w1 >= x2 &&    // r1 right edge past r2 left
        x1 <= x2 + w2 &&    // r1 left edge past r2 right
        y1 + h1 >= y2 &&    // r1 top edge past r2 bottom
        y1 <= y2 + h2) {    // r1 bottom edge past r2 top
            return true;
    }
    return false;
}
// Function to check if a firework has collided with Tom
bool f_collided(double x1, double y1, double x2, double y2, double w2, double h2) {
  // Check if the firework is inside Tom's bounds
  if (x1 >= x2 &&        // if right of the left edge AND
      x1 <= x2 + w2 &&   // if left of the right edge AND
      y1 >= y2 &&        // if below the top AND
      y1 <= y2 + h2) {   // if above the bottom
        return true;
  }
  return false;
}

// Function to draw game over screen
void draw_game_over()
{
    if (game_over == true) // If game over
    {
        if (game_outcome == 'L') // If game lost, draw game lost text
        {
            draw_string((LCD_X / 2) - 22, 5, "You Lose!", FG_COLOUR);
            draw_string((LCD_X / 2) - 40, 20, "Use right button", FG_COLOUR);
            draw_string((LCD_X / 2) - 35, 35, "to play again.", FG_COLOUR);
        }
        if (game_outcome == 'W') // If game won, draw game won text
        {
            draw_string((LCD_X / 2) - 20, 5, "You Win!", FG_COLOUR);
            draw_string((LCD_X / 2) - 40, 20, "Use right button", FG_COLOUR);
            draw_string((LCD_X / 2) - 35, 35, "to play again.", FG_COLOUR);
        }
    }
}

/*-------------Level one functions------------*/
// Storing level one walls
void level_one()
{
    // If game has started
    if (start_game == true && level == 1)
    {
        // Storing wall values for level one at given positions
        walls[0][0] = 18; // Wall 1 x1 position
        walls[0][1] = 15; // Wall 1 y1 position
        walls[0][2] = 13; // Wall 1 x2 position
        walls[0][3] = 25; // Wall 1 y2 position
        walls[1][0] = 25; // Wall 2 x1 position
        walls[1][1] = 35; // Wall 2 y1 position
        walls[1][2] = 25; // Wall 2 x2 position
        walls[1][3] = 45; // Wall 2 y2 position
        walls[2][0] = 45; // Wall 3 x1 position
        walls[2][1] = 10; // Wall 3 y1 position
        walls[2][2] = 60; // Wall 3 x2 position
        walls[2][3] = 10; // Wall 3 y2 position
        walls[4][0] = 58; // Wall 4 x1 position
        walls[4][1] = 25; // Wall 4 y1 position
        walls[4][2] = 72; // Wall 4 x2 position
        walls[4][3] = 30; // Wall 4 y2 position
    }
}

/*-------------Tom functions------------*/
// Draw Tom function
void draw_t(double x, double y)
{
    // If game has started
    if (start_game == true)
    {
        // Draw pixels for Tom's bitmap at given x and y position (T shape)
        draw_pixel(x, y, FG_COLOUR);
        draw_pixel(x + 1, y, FG_COLOUR);
        draw_pixel(x + 2, y, FG_COLOUR);
        draw_pixel(x + 1, y + 1, FG_COLOUR);
        draw_pixel(x + 1, y + 2, FG_COLOUR);
        draw_pixel(x + 1, y + 3, FG_COLOUR);
        draw_pixel(x + 1, y + 4, FG_COLOUR);
    }
}

// Function to set up Tom
void setup_t()
{
    double t_dir = rand() * (M_PI / 2) / RAND_MAX; // Setting Tom's direction to random
    t_dx = step * cos(t_dir);                    // Setting Tom's initial change in x pos
    t_dy = step * sin(t_dir);                    // Setting Tom's initial change in y pos
}

// Move Tom function
void move_t(){
    // If game has not been paused
    if (!pause_game){
        // Assuming that Tom has not already collided with the borders.
        // Predicting the next screen position of Tom
        double new_x = round(t_x + t_dx); // Tom predicted x pos = Tom current x pos + Tom change in x pos
        double new_y = round(t_y + t_dy); // Tom predicted y pos = Tom current y pos + Tom change in y pos
        bool bounced = false;              // Check if Tom has bounced wall
        // If Tom collided with left or right of screen
        if (new_x < 0 || new_x > LCD_X - t_w)
        {
            // Bounce off left or right wall: reverse horizontal direction
            t_dx = -t_dx;
            bounced = true;
        }
        // If Tom collided with top or bottom of screen
        if ((new_y == s_h || new_y == (LCD_Y - t_h)))
        {
            // Bounce off top or bottom wall: reverse vertical direction
            t_dy = -t_dy;
            bounced = true;
        }
        // If Tom collided with Jerry
        if (c_collided(t_x, t_y, t_w, t_h, j_x, j_y, j_w, j_h))
        {
            if (j_lives > 0)
            {
                j_lives--; // Decrease life count
            }
            j_hit2 = true; // Set Jerry hit check to true
        }
        // If Tom collided with walls above or below
        if ((w_collided(t_x, t_y - t_dy, t_w, t_h)) || (w_collided(t_x, t_y + t_dy, t_w, t_h)))
        {
            // Bounce off top wall: reverse vertical direction
            t_dy = -t_dy;
        }
        // If Tom collided with walls to left or right
        if ((w_collided(t_x - t_dx, t_y, t_w, t_h)) || (w_collided(t_x + t_dx, t_y, t_w, t_h)))
        {
            // Bounce off left wall: reverse horizontal direction
            t_dx = -t_dx;
        }
        // If Tom has not bounced
        if (!bounced)
        {
            // Keep moving Tom
            t_x += t_dx;
            t_y += t_dy;
        }
    }
}

/*-------------Jerry functions------------*/
// Draw Jerry function
void draw_j(double x, double y)
{
    // If game has started
    if (start_game == true)
    {
        // Draw pixels for Jerry's bitmap at given x and y position (J shape)
        draw_pixel(x, y, FG_COLOUR);
        draw_pixel(x + 1, y, FG_COLOUR);
        draw_pixel(x + 2, y, FG_COLOUR);
        draw_pixel(x + 2, y + 1, FG_COLOUR);
        draw_pixel(x + 2, y + 2, FG_COLOUR);
        draw_pixel(x + 2, y + 3, FG_COLOUR);
        draw_pixel(x + 1, y + 4, FG_COLOUR);
        draw_pixel(x, y + 4, FG_COLOUR);
    }
}
// Move Jerry function
void move_j(){
    
        // If up stick is pressed and Jerry has not moved past status bar or wall above
        if(BIT_IS_SET(PIND, 1) && (j_y > s_h) && !(w_collided(j_x, j_y - 1, j_w, j_h))){
            j_y--;
        }
        // If down stick is pressed and Jerry has not moved past border or wall below
        if(BIT_IS_SET(PINB, 7) && j_y < (LCD_Y - j_h) && !(w_collided(j_x, j_y + 1, j_w, j_h + 1))){
            j_y++;
        }
        // If left stick is pressed and Jerry has not moved past border or wall to left
        if(BIT_IS_SET(PINB, 1) && (j_x > 0) && !(w_collided(j_x - 1, j_y, j_w, j_h))){
            j_x--;
        }
        // If right stick is pressed and Jerry has not moved past border or wall to right
        if(BIT_IS_SET(PIND, 0) && (j_x < (LCD_X - j_w)) && !(w_collided(j_x + 1, j_y, j_w + 1, j_h))){
            j_x++;
        }
}
void hit_j()
{
    // If Jerry hasn't lost all lives
    if (j_lives > 0)
    {
        // If Jerry has been hit by a mousetrap
        if (j_hit == true)
        {
            j_x = j_sx; // Set Jerry X position to spawn X position
            j_y = j_sy; // Set Jerry Y position to spawn Y position
            j_hit = false;      // Set Jerry hit check to false
        }
        // If Jerry has been hit by Tom
        else if (j_hit2 == true)
        {
            j_x = j_sx; // Set Jerry X position to spawn X position
            j_y = j_sy; // Set Jerry Y position to spawn Y position
            t_x = t_sx;     // Set Tom X position to spawn X position
            t_y = t_sy;     // Set Tom Y position to spawn Y position
            j_hit2 = false;     // Set Jerry hit check to false
        }
        // Otherwise Jerry hit check remains false
        else
        {
            j_hit = false;
            j_hit2 = false;
        }
    }
}

// Function to determine if Jerry has lost the game
void j_lose()
{
    if (j_lives == 0)
    {
        game_outcome = 'L';
        j_score = 0;
        game_over = true;
    }
}

/*-------------Map functions------------*/
// Drawing walls
void draw_walls(){
    // If game has started
    if (start_game == true)
    {
        // Looping through walls array
        for (int i = 0; i < map_size; i++)
        {
            if (walls[i][0] != 0 && walls[i][1] != 0 && walls[i][2] != 0 && walls[i][3] != 0) // If wall index is not empty
            {
                draw_line(walls[i][0], walls[i][1], walls[i][2], walls[i][3], FG_COLOUR); // Draw wall
            }
        }
    }
}

/*-------------Jerry Weapon functions------------*/
// Function to set up weapons
void setup_weapons()
{
    if (weapons_total < max_w)
    {
        weapon_x[weapons_total] = j_x + 2;                                // Storing weapon x position
        weapon_y[weapons_total] = j_y + 2;                                // Storing weapon y position
        char_xdist = abs(round(t_x - weapon_x[weapons_total]));         // Storing x distance between weapon and Tom
        char_ydist = abs(round(t_y - weapon_y[weapons_total]));         // Storing y distance between weapon and Tom
        char_dist = round(sqrt(pow(char_xdist, 2) + pow(char_ydist, 2))); // Calculating displacement between Tom and weapon
        const double w_speed = 1.5;                                      // Setting weapon optimal speed magnitude
        weapon_dx = w_speed * cos(char_xdist / char_dist);                // Setting Tom's initial change in x pos
        weapon_dy = w_speed * sin(char_ydist / char_dist);                // Setting Tom's initial change in y pos
        weapons_total++;                                                  // Increase total weapon count
    }
}

// Function to remove weapon
void rm_weapons(int index)
{
    // Loop through door count
    for (int i = index; i < (weapons_total - 1); i++)
    {
        weapon_x[i] = weapon_x[i + 1]; // Set current weapon x position to next x position
        weapon_y[i] = weapon_y[i + 1]; // Set current weapon y position to next y position
    }
    weapons_total--; // Reduce total weapon count
}

// Draw weapon bitmap function
void draw_f(double x, double y)
{
    // If game has started
    if (start_game == true)
    {
        // Draw pixels for firework's bitmap at given x and y position (single pixel)
        draw_pixel(x, y, FG_COLOUR);
        
    }
}

// Function to draw weapon
void draw_weapons()
{
    // Loop through number of weapons available
    for (int i = 0; i < weapons_total; i++)
    {
        if (weapon_fired == true) // If weapon fired
        {
            draw_f(weapon_x[i], weapon_y[i]); // Draw weapon at specified location
        }
    }
}

// Function to fire weapon
void fire_weapon()
{
    if (weapon_fired) // If weapon is fired
    {
        for (int i = 0; i < weapons_total; i++) // Loop through weapons array
        {
            if (t_x <= weapon_x[i]) // If Tom is left to weapon
            {
                weapon_x[i] -= weapon_dx; // Fire weapon to left
            }
            if (t_x >= weapon_x[i]) // If Tom is right to weapon
            {
                weapon_x[i] += weapon_dx; // Fire weapon to right
            }
            if (t_y >= weapon_y[i]) // If Tom is below weapon
            {
                weapon_y[i] += weapon_dy; // Fire weapon down
            }
            if (t_y <= weapon_y[i]) // If Tom is above weapon
            {
                weapon_y[i] -= weapon_dy; // Fire weapon up
            }
        }
    }
}

// Function to update weapons status
void update_weapons()
{
    setup_weapons();
    // Loop through number of weapons available
    for (int i = 0; i < weapons_total; i++)
    {
        if (f_collided(weapon_x[i], weapon_y[i], t_x, t_y, t_w, t_h) && weapon_fired) // If Tom position is equal to weapon position and weapon has been fired
        {
            rm_weapons(i);        // Remove weapon
            j_score++;            // Increase Jerry score
            t_x = t_sx;   // Reset Tom x position
            t_y = t_sy;   // Reset Tom y position
            weapon_fired = false; // Reset weapon fired check
        }
        if (w_collided(weapon_x[i], weapon_y[i], 1, 1) && weapon_fired) // If weapon collided with walls and weapon has been fired
        {
            rm_weapons(i);        // Remove weapon
            weapon_fired = false; // Reset weapon fired check
        }
        if ((weapon_y[i] <= s_h || weapon_y[i] >= LCD_Y - 1) && weapon_fired) // If weapon position is equal to top or bottom of game screen and weapon has been fired
        {
            rm_weapons(i);        // Remove weapon
            weapon_fired = false; // Reset weapon fired check
        }
        if ((weapon_x[i] <= 0 || weapon_x[i] >= LCD_X - 1) && weapon_fired) // If weapon position is equal to left or right of game screen and has been fired
        {
            rm_weapons(i);        // Remove weapon
            weapon_fired = false; // Reset weapon fired check
        }
    }
}

/*-------------Cheese functions------------*/
// Function to setup cheese
void setup_c()
{
    if (total_c < max_c)
    {
        c_x[total_c] = 1 + rand() % (LCD_X - 1 - 3);  // Storing cheese x position
        c_y[total_c] = s_h + rand() % (LCD_Y - s_h - 5);    // Storing cheese y position
    }
}
// Draw single cheese function
void draw_c(double x, double y)
{
    // If game has started
    if (start_game == true)
    {
        // Draw pixels for cheese's bitmap at given x and y position (C shape)
        draw_pixel(x + 1, y, FG_COLOUR);
        draw_pixel(x, y + 1, FG_COLOUR);
        draw_pixel(x + 2, y + 1, FG_COLOUR);
        draw_pixel(x, y + 2, FG_COLOUR);
        draw_pixel(x, y + 3, FG_COLOUR);
        draw_pixel(x + 2, y + 3, FG_COLOUR);
        draw_pixel(x + 1, y + 4, FG_COLOUR);
    }
}
// Function to remove cheese
void rm_c(int index)
{
    // Loop through cheese count
    for (int i = index; i < (total_c - 1); i++)
    {
        c_x[i] = c_x[i + 1]; // Set current cheese x position to next x position
        c_y[i] = c_y[i + 1]; // Set current cheese y position to next y position
    }
    total_c--; // Reduce cheese count by 1
}

// Function to draw all cheese
void draw_cheese()
{
    // Loop through number of cheese available
    for (int i = 0; i < total_c; i++)
    {
        draw_c(c_x[i], c_y[i]); // Draw cheese at specified locations
    }
}

// Function to update cheese upon interaction
void update_c()
{
    { // If Jerry is the player
        setup_c();
        // Loop through number of cheese available
        for (int i = 0; i < total_c; i++)
        {
            if (c_collided(c_x[i], c_y[i], c_w, c_h, j_x, j_y, j_w, j_h)) // If jerry position is equal to cheese position
            {
                rm_c(i); // Remove cheese
                j_score++;    // Increase score
            }
            if (w_collided(c_x[i], c_y[i], c_w, c_h)) // If cheese collided with walls
            {
                rm_c(i); // Remove cheese
            }
            
        }
    }
}

/*-------------Mouse trap functions------------*/
// Set up traps function
void setup_traps()
{
    // If trap total is less than max and Tom is not colliding with any items
    if (total_traps < max_t && !(c_collided(t_x, t_y, t_w, t_h, c_x[total_c], c_y[total_c], c_w, c_h)))
    {
        traps_x[total_traps] = t_x;  // Storing trap x position
        traps_y[total_traps] = t_y;    // Storing trap y position
    }
}

// Function to remove traps
void rm_traps(int index)
{
    // Loop through traps count
    for (int i = index; i < (total_traps - 1); i++)
    {
        traps_x[i] = traps_x[i + 1]; // Set current trap x position to next x position
        traps_y[i] = traps_y[i + 1]; // Set current trap y position to next y position
    }
}
// Draw single trap function
void draw_trap(double x, double y)
{
    // If game has started
    if (start_game == true)
    {
        // Draw pixels for trap's bitmap at given x and y position (X shape)
        draw_pixel(x, y, FG_COLOUR);
        draw_pixel(x + 2, y, FG_COLOUR);
        draw_pixel(x, y + 1, FG_COLOUR);
        draw_pixel(x + 2, y + 1, FG_COLOUR);
        draw_pixel(x + 1, y + 2, FG_COLOUR);
        draw_pixel(x, y + 3, FG_COLOUR);
        draw_pixel(x + 2, y + 3, FG_COLOUR);
        draw_pixel(x, y + 4, FG_COLOUR);
        draw_pixel(x + 2, y + 4, FG_COLOUR);
    }
}
// Function to draw traps
void draw_traps()
{
    // Loop through number of traps available
    for (int i = 0; i < total_traps; i++)
    {
        draw_trap(traps_x[i], traps_y[i]); // Draw traps at specified locations
    }
}
// Function to update traps upon interaction
void update_traps()
{
    setup_traps();
    // Loop through number of traps available
    for (int i = 0; i < total_traps; i++)
    {
        if (c_collided(j_x, j_y, j_w, j_h, traps_x[i], traps_y[i], traps_w, traps_h)) // If Jerry position is equal to traps position
        {
            rm_traps(i); // Remove trap
            if (j_lives > 0)
            {
                j_lives--; // Decrease life count
            }
            if (total_traps > 0)
            {
                total_traps--; // Reduce trap count by 1
            }
            j_hit = true; // Set Jerry hit check to true
        }
        if (w_collided(traps_x[i], traps_y[i], traps_w, traps_h)) // If traps collided with walls
        {
            rm_traps(i); // Remove trap
        }
    }
}
/*-------------Door Appearance functions------------*/
// Function to setup door
void setup_door()
{
    if (door_total == 0 && j_score >= 5) // If door total is 0 and score is greater than 5
    {
        door_x[door_total] = 1 + rand() % (LCD_X - 1 - 3);  // Storing door x position
        door_y[door_total] = s_h + rand() % (LCD_Y - s_h - 5);    // Storing door y position
        door_total++;
    }
}

// Function to remove door
void rm_door(int index)
{
    // Loop through door count
    for (int i = index; i < (door_total - 1); i++)
    {
        door_x[i] = door_x[i + 1]; // Set current door x position to next x position
        door_y[i] = door_y[i + 1]; // Set current door y position to next y position
    }
    door_total--;
}

// Draw door bitmap function
void draw_d(double x, double y)
{
    // If game has started
    if (start_game == true)
    {
        // Draw pixels for Door's bitmap at given x and y position (Door shape)
        draw_pixel(x + 1, y, FG_COLOUR);
        draw_pixel(x, y + 1, FG_COLOUR);
        draw_pixel(x + 2, y + 1, FG_COLOUR);
        draw_pixel(x, y + 2, FG_COLOUR);
        draw_pixel(x + 2, y + 2, FG_COLOUR);
        draw_pixel(x, y + 3, FG_COLOUR);
        draw_pixel(x + 2, y + 3, FG_COLOUR);
        draw_pixel(x, y + 4, FG_COLOUR);
        draw_pixel(x + 1, y + 4, FG_COLOUR);
        draw_pixel(x + 2, y + 4, FG_COLOUR);
    }
}

// Function to draw door
void draw_door()
{
    // Loop through number of doors available
    for (int i = 0; i < door_total; i++)
    {
        draw_d(door_x[i], door_y[i]); // Draw door at specified location
    }
}

void update_door()
{
    setup_door();
    // Loop through number of doors available
    for (int i = 0; i < door_total; i++)
    {
        if (c_collided(j_x, j_y, j_w, j_h,  door_x[i], door_y[i], door_w, door_h)) // If Jerry position is equal to door position
        {
            rm_door(i); // Remove door
            if (level == level_count)
            { // If current level is last in array
                game_over = true;
                game_outcome = 'W';
            }
            // If level is before final level
            else if (level < level_count)
            {
                level++;           // Increase level
                next_level = true; // Set next level to true
            }
        }
        if (w_collided(door_x[i], door_y[i], door_w, door_h)) // If door collided with walls
        {
            rm_door(i); // Remove door
        }
        // Loop through number of cheese available
        for (int j = 0; j < total_c; j++)
        {
            if (c_collided(door_x[i], door_y[i], door_w, door_h, c_x[j], c_y[j], c_w, c_h)) // If door position is same as cheese position
            {
                rm_door(i); // Remove door
            }
        }
        // Loop through number of traps available
        for (int j = 0; j < total_traps; j++)
        {
            if (c_collided(door_x[i], door_y[i], door_w, door_h, traps_x[j], traps_y[j], traps_w, traps_h)) // If door position is same as traps position
            {
                rm_door(i); // Remove door
            }
        }
    }
}

/*-------------Main functions------------*/

// Function to start next/ restart level
void level_new()
{
    // Load game elements for new level
    if (next_level == true) // If next level is true, reset all game properties
    {
        // Remove all cheese
        for (int i = 0; i < total_c; i++)
        {
            rm_c(i);
        }
        // Remove all traps
        for (int i = 0; i < total_traps; i++)
        {
            rm_traps(i);
        }
        // Remove door
        for (int i = 0; i < door_total; i++)
        {
            rm_door(i);
        }
        // Remove fireworks
        for (int i = 0; i < weapons_total; i++)
        {
            rm_weapons(i);
        }
        // Reset all statuses
        game_over = false;
        total_c = 0;
        j_lives = 5;
        secs = 0;
        mins = 0;
        t_x = t_sx;
        t_y = t_sy;
        // Only applies if level is being reset
        if (restart_level == true)
        {
            j_x = j_sx;
            j_y = j_sy;
            j_score = 0;
        }
        pause_game = false;
        next_level = false;
        restart_level = false;
    }
}

// Function to draw all game elements
void draw_all(){
    if(game_over == false){
        draw_start();
        draw_status();
        draw_t(t_x, t_y);
        draw_j(j_x, j_y);
        level_one();
        draw_cheese();
        draw_traps();
        draw_door();
        //draw_weapons();
        draw_walls();
    }
}

// Function to set up all game elements
void setup(){
    setup_t();
    setup_c();
    setup_traps();
    //setup_weapons();
    setup_door();
}

// Main game loop
void process(void)
{
    // If game has started
    if (start_game == true)
    {
        //	Initialise Timer 1 and Timer 0 in normal mode and enable prescalers
        TCCR1A = 0;
        TCCR1B = 4;
        TCCR0A = 0;
	    TCCR0B = 5; 
        //	Enabling timer overflow for Timer 1 and Timer 0.
        TIMSK1 = 1;
        TIMSK0 = 1; 
        //	Turning on interrupts.
        sei();
        // Enabling movements
        move_j();
        hit_j();
        j_lose();
        move_t();
        // Enabling pause
        pause();
        // Enabling restart game
        restart();
        // Enabling weapon fire button
        //fire();
        // Enabling cheese interactions
        update_c();
        // Enabling mouse trap interactions
        update_traps();
        // Enabling door interactions
        update_door();
        // Enabling weapon interactions
        //update_weapons();
    }
}

// Main program
int main(void)
{
    t_setup(); // Setting up Teensy
    setup(); // Setting up all game elements
    for (;;)
    {
        clear_screen(); // Clearing screen for next frame
        draw_all(); // Drawing all game elements
        draw_game_over(); // Drawing game over screen when triggered
        process(); // Main game loop
        level_new(); // Restart / New level
        show_screen(); // Revealing current frame
    }
    return 0;
}
