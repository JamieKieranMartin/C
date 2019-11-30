#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <math.h>
#include <stdlib.h>
#include "cab202_adc.h"
#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"

#include "usb_serial.h"
#define BUFF_LENGTH 64

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /* pi             */
#endif
#define Y_MAX LCD_Y - 7
#define X_MAX LCD_X - 5
#define FREQ 8000000.0
#define PRESCALE 64.0
#define CHAR_H 4
#define CHAR_W 4
#define BIT(x) (1 << (x))
#define OVERFLOW_TOP (1023)
#define ADC_MAX (1023)

uint8_t actions[7][2] = {0};

// Set this to true when the game is over
bool pause = false, next_level = false, level2 = false;

// step globals
double j_step = 1;
double t_step = 0.5;
double wall_step = 0.4;

// Jerry's position + state
int super_J = 0;
int J_sx = 0, J_sy = 8, J_x, J_y;
int lives, score, levels, level = 0;
#define MAX_LIVES 5
double right_adc, left_adc;

// Tom's position + state
double T_sx = LCD_X - 5, T_sy = LCD_Y - 9, T_x, T_y;
double T_dx, T_dy;

// Item + Object globals
int wall_count = 4, cheese_count, trap_count, firework_count;
int cheese_eaten;
double walls[6][4] = {
    {18, 15, 13, 25},
    {25, 35, 25, 45},
    {45, 10, 60, 10},
    {58, 25, 72, 30},
    {},
    {}};
double cheeses[5][2] = {};
double traps[5][2] = {};
double fireworks[20][2] = {};
int D_x, D_y;
int P_x, P_y;
// Timer globals
int minutes, seconds, milliseconds, total_seconds, last_cheesed, last_trapped, last_firework, last_potion;
uint32_t counter = 0;

// Finds the distance between two points
double distance(double x1, double y1, double x2, double y2)
{
    return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

// LINE/POINT COLLISION
bool linePoint(double x1, double y1, double x2, double y2, double px, double py)
{
    // get distance from the point to the two ends of the line
    double d1 = distance(px, py, x1, y1);
    double d2 = distance(px, py, x2, y2);

    // get the length of the line
    double lineLen = distance(x1, y1, x2, y2);

    // since floats are so minutely accurate, add
    // a little buffer zone that will give collision
    double buffer = 0.1; // higher # = less accurate

    // if the two distances are equal to the line's
    // length, the point is on the line
    if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer)
    {
        return true;
    }
    return false;
}

// POINT/RECTANGLE COLLISION
bool pointRect(double px, double py, double rx, double ry, double rw, double rh)
{
    if (px >= rx &&      // right of the left edge AND
        px <= rx + rw && // left of the right edge AND
        py >= ry &&      // below the top AND
        py <= ry + rh)   // above the bottom
        return true;
    else
        return false;
}

// LINE/LINE COLLISION
bool lineLineCollision(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    // calculate the direction of the lines
    double uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
    double uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
    // if uA and uB are between 0-1, lines are colliding
    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
        return true;
    else
        return false;
}

// LINE/RECTANGLE COLLISION
bool lineRectCollision(int x1, int y1, int x2, int y2, int rx, int ry, int w, int h)
{
    // check if the line has hit any of the rectangle's sides
    // uses the Line/Wall function below
    bool left = lineLineCollision(x1, y1, x2, y2, rx, ry, rx, ry + h);
    bool right = lineLineCollision(x1, y1, x2, y2, rx + w, ry, rx + w, ry + h);
    bool top = lineLineCollision(x1, y1, x2, y2, rx, ry, rx + w, ry);
    bool bottom = lineLineCollision(x1, y1, x2, y2, rx, ry + h, rx + w, ry + h);
    // if ANY of the above are true, the line
    // has hit the rectangle
    if (left || right || top || bottom)
        return true;
    else
        return false;
}

// CHECKS IF COLLIDED WITH A WALL
bool wallCollision(int x, int y, int w, int h)
{
    for (int i = 0; i < wall_count; i++)
    {
        if (lineRectCollision(walls[i][0], walls[i][1], walls[i][2], walls[i][3], round(x), round(y), w, h))
        {
            return true;
            break;
        }
    }
    return false;
}

// CHECKS IF COLLIDED WITH A WALL
bool lineWallCollision(int x, int y)
{
    for (int i = 0; i < wall_count; i++)
    {
        if (linePoint(walls[i][0], walls[i][1], walls[i][2], walls[i][3], x, y))
        {
            return true;
            break;
        }
    }
    return false;
}

// RECTANGLE/RECTANGLE COLLISION
bool rectRectCollision(int ax, int ay, int bx, int by)
{
    if (ax + CHAR_W >= bx && ax <= bx + CHAR_W && ay + CHAR_H >= by && ay <= by + CHAR_H)
    {
        return true;
    }
    return false;
}

// Determines if an area is free for new object
bool spaceTaken(int x, int y) {
    for (int i = 0; i < wall_count; i++)
    {
        if (lineRectCollision(walls[i][0], walls[i][1], walls[i][2], walls[i][3], round(x), round(y), 4, 4))
        {
            return true;
            break;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (traps[i][0] != 0 && traps[i][1] != 0 && rectRectCollision(traps[i][0], traps[i][1], x, y))
        {
            return true;
            break;
        }
        if (cheeses[i][0] != 0 && cheeses[i][1] != 0 && rectRectCollision(cheeses[i][0], cheeses[i][1], x, y)) {
            return true;
            break;
        }
    }

    if (rectRectCollision(J_x, J_y, x, y) || rectRectCollision(D_x, D_y, x, y) || rectRectCollision(P_x, P_y, x, y)) {
        return true;
    }

    return false;
}

// Sets up Jerry, placing initially in the centre of the screen
void setup_J()
{
    J_x = J_sx, J_y = J_sy;
}

// Sets up Tom at a random location and direction
void setup_T()
{
    T_x = T_sx, T_y = T_sy;
    double T_dir = rand() * M_PI * 2 / RAND_MAX;
    T_dx = t_step * cos(T_dir);
    T_dy = t_step * sin(T_dir);
}

// Draws Jerry
void draw_J()
{
    draw_line(J_x, J_y, J_x + CHAR_W, J_y, FG_COLOUR);
    draw_line(J_x + (CHAR_W / 2), J_y, J_x + (CHAR_W / 2), J_y + CHAR_H, FG_COLOUR);
    draw_line(J_x + (CHAR_W / 2), J_y + CHAR_H, J_x, J_y + CHAR_H, FG_COLOUR);
    if (super_J != 0) {
        draw_line(J_x, J_y+1, J_x+CHAR_W, J_y+1, FG_COLOUR);
        draw_line(J_x + (CHAR_W / 2) + 1, J_y, J_x + (CHAR_W / 2) + 1, J_y + CHAR_H, FG_COLOUR);
        draw_line(J_x + (CHAR_W / 2), J_y + CHAR_H - 1, J_x, J_y + CHAR_H - 1, FG_COLOUR);
    }
}

// Updates the position of Jerry based on a key press
void update_J(int c)
{
    double j_s = j_step * ((left_adc + 0.5) * 2);

    // Movement Logic
    if ((actions[0][1] || c == 'w')  && J_y > 8)
    {
        if (super_J == 0) {
            if (wallCollision(J_x, J_y - j_s, CHAR_W, CHAR_H))
                J_y += j_s;
            else if (!wallCollision(J_x, J_y - j_s, CHAR_W, CHAR_H))
                J_y -= j_s;
        } else J_y -= j_s;
    }
    else if ((actions[1][1] || c == 's') && J_y < Y_MAX)
    {
        if (super_J == 0) {
            if (wallCollision(J_x, J_y + j_s, CHAR_W, CHAR_H))
                J_y -= j_s;
            else if (!wallCollision(J_x, J_y - j_s, CHAR_W, CHAR_H))
                J_y += j_s;
        } else J_y += j_s;
    }
    else if ((actions[2][1] || c == 'a') && J_x > 0)
    {
        if (super_J == 0) {
            if (wallCollision(J_x - j_s, J_y, CHAR_W, CHAR_H))
                J_x += j_s;
            else if (!wallCollision(J_x - j_s, J_y, CHAR_W, CHAR_H))
                J_x -= j_s;
        } else J_x -= j_s;
    }
    else if ((actions[3][1] || c == 'd') && J_x < X_MAX)
    {
        if (super_J == 0) {
            if (wallCollision(J_x + j_s, J_y, CHAR_W, CHAR_H))
                J_x -= j_s;
            else if (!wallCollision(J_x + j_s, J_y, CHAR_W, CHAR_H))
                J_x += j_s;
        } else J_x += j_s; 
    }

    // Resets J if out of bounds or has collided with wall
    if ((J_y < 8 || J_y > LCD_Y || J_x < 0 || J_x > LCD_X) || (wallCollision(J_x + 1, J_y + 1, CHAR_W - 1, CHAR_H - 1) && super_J == 0))
    {
        if (J_x != J_sx && J_y != J_sy) {
            setup_J();
            lives--;
        }
    }

    // Checks collision with traps and cheese
    for (int i = 0; i < 5; i++)
    {
        if (rectRectCollision(J_x, J_y, cheeses[i][0], cheeses[i][1]))
        {
            cheeses[i][0] = 0, cheeses[i][1] = 0;
            last_cheesed = total_seconds;
            score++, cheese_count--, cheese_eaten++;
            break;
        }

        if (rectRectCollision(J_x, J_y, traps[i][0], traps[i][1]) && super_J == 0)
        {
            traps[i][0] = 0, traps[i][1] = 0;
            last_trapped = total_seconds;
            lives--, trap_count--;
            break;
        }
    }

    // Checks collision with Door and handles
    if (rectRectCollision(J_x, J_y, D_x, D_y)) {
        D_x = D_y = 0;
        next_level = true;
    }
    // Checks collision with Potion and handles
    if (rectRectCollision(J_x, J_y, P_x, P_y)) {
        last_potion = total_seconds;
        P_x = P_y = 0;
        super_J = 10;
    }
    // Checks collision with Tom and handles
    if (rectRectCollision(J_x, J_y, T_x, T_y))
    {
        if (super_J == 0) {
            setup_J();
            lives--;
        } else if (super_J != 0) {
            setup_T();
            score++;
        }
    }
}

// Draws Tom
void draw_T()
{
    draw_line(T_x, T_y, T_x + CHAR_W, T_y, FG_COLOUR);
    draw_line(T_x + (CHAR_W / 2), T_y, T_x + (CHAR_W / 2), T_y + CHAR_H, FG_COLOUR);
}

// Updates Tom's position
void update_T()
{
    int new_x = round(T_x + T_dx);
    int new_y = round(T_y + T_dy);
    bool bounced = false;
    // Movement Logic
    if (new_x == 0 || new_x == X_MAX + 1 || wallCollision(new_x, new_y, CHAR_W, CHAR_H))
    {
        T_dx = -T_dx;
        bounced = true;
    }

    if (new_y == 8 || new_y == Y_MAX + 1 || wallCollision(new_x, new_y, CHAR_W, CHAR_H))
    {
        T_dy = -T_dy;
        bounced = true;
    }

    if (!bounced)
    {
        T_x += ((left_adc + 0.25) * 2) * T_dx;
        T_y += ((left_adc + 0.25) * 2) * T_dy;
    }

    // Resets Tom if out of bounds
    if (T_y < 8 || T_y > LCD_Y || T_x < 0 || T_x > LCD_X)
    {
        setup_T();
        score++;
    }

    // Resets Tom if collided with walls 
    if (wallCollision(T_x + 1, T_y + 1, CHAR_W - 1, CHAR_H - 1) && T_x != T_sx && T_y != T_sx) {
        setup_T();
    }
}

//	An interrupt service routine to process timer overflow
ISR(TIMER1_OVF_vect)
{
    if (!pause)
    {
        counter++;
        if (((counter * 65536.0 + TCNT1) * PRESCALE / FREQ) >= 1)
        {
            total_seconds++;
            seconds++;
            counter = 0;
            if (super_J != 0) {
                last_potion = total_seconds;
                super_J--;
            }
        }
        if (seconds > 59)
        {
            minutes++;
            seconds = 0;
        }
    }
}

// Gets bit value from controls
bool is_set(int i)
{
    bool is = false;
    switch (i)
    {
    case 0:
        is = BIT_IS_SET(PIND, 1); // JOYSTICK UP
        break;
    case 1:
        is = BIT_IS_SET(PINB, 7); // JOYSTICK DOWN
        break;
    case 2:
        is = BIT_IS_SET(PINB, 1); // JOYSTICK LEFT
        break;
    case 3:
        is = BIT_IS_SET(PIND, 0); // JOYSTICK RIGHT
        break;
    case 4:
        is = BIT_IS_SET(PINB, 0); // JOYSTICK CENTRE
        break;
    case 5:
        is = BIT_IS_SET(PINF, 5); // RIGHT BUTTON
        break;
    case 6:
        is = BIT_IS_SET(PINF, 6); // LEFT BUTTON
        break;
    }
    return is;
}

//	An interrupt service routine to process timer overflow interrupts for Timer 0
ISR(TIMER0_OVF_vect)
{
    for (int i = 0; i < 7; i++)
    {
        bool j = is_set(i);
        uint8_t mask = 0b00000011;
        if (i > 4)
            mask = 0b00000000;
        actions[i][0] = ((actions[i][0] << 1) & mask) | j;
        if (actions[i][0] == mask)
            actions[i][1] = 1;
        else if (actions[i][0] == 0)
            actions[i][1] = 0;
    }
}

char buffer[20];
// Status bar drawer
void status_bar()
{
    snprintf(buffer, sizeof(buffer), "%d L:%d S:%d %02d:%02d", level, lives, score, minutes, seconds);
    draw_string(0, 0, buffer, FG_COLOUR);
    draw_line(0, 7, LCD_X, 7, FG_COLOUR);
}

// Generates a random position in the field of play
int random_position(char type)
{
    if (type == 'y')
        return (rand() % (Y_MAX + 1 - 8)) + 8;
    else if (type == 'x')
        return (rand() % (X_MAX + 1 - 0)) + 0;
    else
        return -1;
}

// Draws all walls
void draw_walls()
{
    for (int i = 0; i < wall_count; i++)
    {
        draw_line(walls[i][0], walls[i][1], walls[i][2], walls[i][3], FG_COLOUR);
    }
}

// Handles the wall movement and wrapping around the bounds
void move_walls()
{
    for (int i = 0; i < wall_count; i++)
    {
        double wall_dx = (walls[i][2] - walls[i][0]);
        double wall_dy = (walls[i][3] - walls[i][1]);

        if (wall_dx < 0) wall_dx *= -1;
        if (wall_dy < 0) wall_dy *= -1;

        walls[i][0] = walls[i][0] + (right_adc * wall_step * sin(wall_dy));
        walls[i][1] = walls[i][1] + (right_adc * wall_step * sin(wall_dx));
        walls[i][2] = walls[i][2] + (right_adc * wall_step * sin(wall_dy));
        walls[i][3] = walls[i][3] + (right_adc * wall_step * sin(wall_dx));

        // if horizontal wall
        if (wall_dx == 0) 
        {
            if (walls[i][0] < 0) walls[i][0] = LCD_X;
            if (walls[i][2] < 0) walls[i][2] = LCD_X;
            if (walls[i][0] > LCD_X) walls[i][0] = 0;
            if (walls[i][2] > LCD_X) walls[i][2] = 0;
        } 
        // if vertical wall
        else if (wall_dy == 0) 
        {
            if (walls[i][1] < 8) walls[i][1] = LCD_Y;
            if (walls[i][3] < 8) walls[i][3] = LCD_Y;
            if (walls[i][1] > LCD_Y) walls[i][1] = 8;
            if (walls[i][3] > LCD_Y) walls[i][3] = 8;
        } 
        // if slanted wall
        else {
            if (walls[i][0] < -10 || walls[i][2] < -10) 
            {
                walls[i][0] += (LCD_X);
                walls[i][2] += (LCD_X);
            }
            else if (walls[i][0] > LCD_X + 10 || walls[i][2] > LCD_X + 10)
            {
                walls[i][0] -= (LCD_X);
                walls[i][2] -= (LCD_X);
            }
            else if (walls[i][1] < 0 || walls[i][3] < 0) 
            {
                walls[i][1] += (LCD_Y);
                walls[i][3] += (LCD_Y);
            }
            else if (walls[i][1] > LCD_Y + 10 || walls[i][3] > LCD_Y + 10)
            {
                walls[i][1] -= (LCD_Y);
                walls[i][3] -= (LCD_Y); 
            }
        }
    }
}

// Draws Firework
void draw_F()
{
    for (int i = 0; i < 20; i++)
    {
        if (fireworks[i][0] != 0 && fireworks[i][1] != 0)
            draw_pixel(fireworks[i][0], fireworks[i][1], FG_COLOUR);
    }
}

// Moves the Firework provided it is active and checks for collision with Tom
void update_F()
{
    double step = 0.5;
    for (int i = 0; i < 20; i++)
    {
        if (fireworks[i][0] != 0 && fireworks[i][1] != 0)
        {
            int x_diff = fireworks[i][0] - T_x, y_diff = fireworks[i][1] - T_y;
            // if T + F collided reset T and F, add score
            if (pointRect(fireworks[i][0], fireworks[i][1], T_x - 1, T_y - 1, CHAR_W + 1, CHAR_H + 1))
            {
                fireworks[i][0] = fireworks[i][1] = 0;
                firework_count--;
                setup_T();
                score++;
                break;
            }
            // if the difference is less than 0, meaning F is to the left of T
            if (x_diff < 0)
            {
                // only move right if the new x is not a wall, otherwise destory F
                if (lineWallCollision(fireworks[i][0] + step, fireworks[i][1]))
                {
                    fireworks[i][0] = fireworks[i][1] = 0;
                    firework_count--;
                    break;
                }
                else
                    fireworks[i][0] = fireworks[i][0] + step;
            }
            // if the difference is greater than 0, meaning F is to the right of T
            else if (x_diff > 0)
            {
                // only move left if the new x is not a wall, otherwise destory F
                if (lineWallCollision(fireworks[i][0] - step, fireworks[i][1]))
                {
                    fireworks[i][0] = fireworks[i][1] = 0;
                    firework_count--;
                    break;
                }
                else
                    fireworks[i][0] = fireworks[i][0] - step;
            }

            // if the difference is less than 0, meaning F is below T
            if (y_diff < 0)
            {
                // only move down if the new x is not a wall, otherwise destory F
                if (lineWallCollision(fireworks[i][0], fireworks[i][1] + step))
                {
                    fireworks[i][0] = fireworks[i][1] = 0;
                    firework_count--;
                    break;
                }
                else
                    fireworks[i][1] = fireworks[i][1] + step;
            }
            // if the difference is greater than 0, meaning F is above T
            else if (y_diff > 0)
            {
                // only move up if the new x is not a wall, otherwise destory F
                if (lineWallCollision(fireworks[i][0], fireworks[i][1] - step))
                {
                    fireworks[i][0] = fireworks[i][1] = 0;
                    firework_count--;
                    break;
                }
                else
                    fireworks[i][1] = fireworks[i][1] - step;
            }
        }
    }
}

//	Draws all the cheeses in the cheese array
void draw_cheeses()
{
    for (int i = 0; i < 5; i++)
    {
        if (cheeses[i][0] != 0 && cheeses[i][1] != 0)
        {
            draw_line(cheeses[i][0], cheeses[i][1], cheeses[i][0] + CHAR_W, cheeses[i][1] + (CHAR_H / 2), FG_COLOUR);
            draw_line(cheeses[i][0], cheeses[i][1], cheeses[i][0], cheeses[i][1] + CHAR_H, FG_COLOUR);
            draw_line(cheeses[i][0], cheeses[i][1] + CHAR_H, cheeses[i][0] + CHAR_W, cheeses[i][1] + (CHAR_H / 2), FG_COLOUR);
        }
    }
}

//	Tries to drop a cheese once the time is right, provided there aren't already 5 cheeses
void try_drop_cheese()
{
    if (cheese_count < 5 && total_seconds - last_cheesed >= 2)
    {
        cheese_count++;
        last_cheesed = total_seconds;
        // loops through cheeses and generates random position
        for (int i = 0; i < 5; i++)
        {
            if (cheeses[i][0] == 0 && cheeses[i][1] == 0)
            {
                int new_x = random_position('x'), new_y = random_position('y');
                while (spaceTaken(new_x, new_y)) {
                    new_x = random_position('x');
                    new_y = random_position('y');
                }
                cheeses[i][0] = new_x, cheeses[i][1] = new_y;
                break;
            }
        }
    }
}

//  Draws all the traps in the trap array
void draw_traps()
{
    for (int i = 0; i < 5; i++)
    {
        if (traps[i][0] != 0 && traps[i][1] != 0)
        {
            draw_line(traps[i][0], traps[i][1], traps[i][0] + CHAR_W, traps[i][1] + CHAR_H, FG_COLOUR);
            draw_line(traps[i][0] + CHAR_W, traps[i][1], traps[i][0], traps[i][1] + CHAR_H, FG_COLOUR);
        }
    }
}

//	Tries to drop a mousetrap once the time is right, provided there aren't already 5 traps
void try_drop_trap()
{
    if (trap_count < 5 && total_seconds - last_trapped >= 3)
    {
        // loops through traps and generates random position
        for (int i = 0; i < 5; i++)
        {
            if (traps[i][0] == 0 && traps[i][1] == 0 && !spaceTaken(T_x, T_y))
            {
                trap_count++;
                last_trapped = total_seconds;
                traps[i][0] = T_x, traps[i][1] = T_y;
                break;
            }
        }
    }
}

//	Try open the door for the next level
void try_drop_door()
{
    if (cheese_eaten >= 5 && D_x == 0 && D_y == 0)
    {
        int new_x = random_position('x'), new_y = random_position('y');
        while (spaceTaken(new_x, new_y)) {
            new_x = random_position('x');
            new_y = random_position('y');
        }
        D_x = new_x, D_y = new_y;
    }
}

//	Draw the door for each loop
void draw_door()
{
    if (D_x != 0 && D_y != 0)
    {
        draw_line(D_x, D_y, D_x + CHAR_W, D_y, FG_COLOUR);
        draw_line(D_x, D_y, D_x, D_y + CHAR_H, FG_COLOUR);
        draw_line(D_x + CHAR_W, D_y, D_x + CHAR_W, D_y + CHAR_H, FG_COLOUR);
        draw_line(D_x, D_y + CHAR_H, D_x + CHAR_W, D_y + CHAR_H, FG_COLOUR);
        draw_pixel(D_x + (CHAR_W - (CHAR_W / 3)), D_y + (CHAR_H / 2), FG_COLOUR);
    }
}

// Tries to drop the potion
void try_drop_potion()
{
    if (P_x == 0 && P_y == 0 && total_seconds - last_potion >= 5 && !spaceTaken(T_x, T_y))
    {
        last_potion = total_seconds;
        P_x = T_x, P_y = T_y;
    }
}

// Draws the potion
void draw_potion()
{
    if (P_x != 0 && P_y != 0)
    {
        draw_line(P_x + (CHAR_W / 2), P_y, P_x + (CHAR_W / 2), P_y + CHAR_H, FG_COLOUR);
        draw_line(P_x, P_y + (CHAR_H / 2), P_x + CHAR_W, P_y + (CHAR_H / 2), FG_COLOUR);
    }
}

// Draws all things
void draw_all()
{
    status_bar();
    draw_walls();
    draw_door();
    draw_cheeses();
    draw_traps();
    draw_potion();
    draw_F();
    draw_J();
    draw_T();
}

// Sends to USB serial connection
void usb_serial_send(char * message) {
	usb_serial_write((uint8_t *) message, strlen(message));
}

// Reads from USB serial connection
void usb_serial_read_string(char * message){
   int c = 0;
   int buffer_count=0;
      
   while (c != '\n'){
         c = usb_serial_getchar();
         message[buffer_count]=c;
         buffer_count++;
    }
}

// Handles each process
void process(void)
{
    // Logic for Level 2
    int c = 0;
    char tx_buffer1[32];
    if (level == 2) {
        // Initialise the USB serial
        if (!level2) {
            cheese_eaten = 0;
            wall_count = 0;
            usb_init();
            while ( !usb_configured() ) {
                // Block until USB is ready.
            }
        
            for (int i = 0; i < 4; i++)
            {
                walls[i][0] = 0;
                walls[i][1] = 0;
                walls[i][2] = 0;
                walls[i][3] = 0;
            }

            level2 = true;
        }
        
        if (usb_serial_available()) {
            c = usb_serial_getchar();
            snprintf( tx_buffer1, sizeof(tx_buffer1), "received '%c'\r\n", c );
	        usb_serial_send( tx_buffer1 );
        }

        if (c =='T')
        {
            usb_serial_read_string(tx_buffer1);
            usb_serial_send( tx_buffer1 );
            int x = T_sx;
            int y = T_sy;
            sscanf( tx_buffer1, "%d %d", &x, &y);
            T_x = x;
            T_y = y;   
        }

        if (c =='J')
        {
            usb_serial_read_string(tx_buffer1);
            usb_serial_send( tx_buffer1 );
            int x = J_sx;
            int y = J_sy;
            sscanf( tx_buffer1, "%d %d", &x, &y);
            J_x = x;
            J_y = y;   
        }

        if (c =='W' && wall_count < 6)
        {
            char wallsc[64];
            usb_serial_read_string(wallsc);
            usb_serial_send( wallsc ); 
            int wall1, wall2, wall3, wall4;
            sscanf( wallsc, "%d %d %d %d", &wall1, &wall2, &wall3, &wall4);
            wall_count++;
            walls[wall_count - 1][0] = wall1; 
            walls[wall_count - 1][1] = wall2;
            walls[wall_count - 1][2] = wall3; 
            walls[wall_count - 1][3] = wall4;
        }
	}
    // End of Level 2 Logic

    update_J(c);
    update_F();
    try_drop_door();

    if (!pause)
    {
        move_walls();
        try_drop_cheese();
        try_drop_potion();
        try_drop_trap();
        update_T();
    }

    // Serial Logic
    if (c == 'i') {
        char tx_buffer[BUFF_LENGTH];
        snprintf(tx_buffer, sizeof(tx_buffer), "%02d:%02d L:%d L:%d S:%d F:%d T:%d C:%d CE:%d SuperJ:%d P:%d\r\n", minutes, seconds, level, lives, score, firework_count, trap_count, cheese_count, cheese_eaten, ((super_J != 0) ? 1 : 0 ), (pause ? 1 : 0));
        usb_serial_send(tx_buffer);
    }

    if (actions[6][0] || c == 'l')
    {
        _delay_ms(70);
        next_level = true;
    }

    // RIGHT BUTTON
    if (actions[5][0] || c == 'p')
    {
        _delay_ms(70);
        draw_line(0, LCD_Y - 1, LCD_X, LCD_Y - 1, FG_COLOUR);
        pause = !pause;
    }
    // CENTER JOYSTICK
    if (actions[4][1] || c == 'f')
    {
        _delay_ms(70);
        if (firework_count < 20 && last_firework <= total_seconds - 0.5 && cheese_eaten >= 3)
        {
            for (int i = 0; i < 20; i++)
            {
                if (fireworks[i][0] == 0 && fireworks[i][1] == 0)
                {
                    fireworks[i][0] = J_x;
                    fireworks[i][1] = J_y;
                    last_firework = total_seconds;
                    firework_count++;
                    break;
                }
            }
        }
    }

    draw_all();
}

// Configures Timer
void setup_timers(void)
{
    // PWM INITIALISE
    TC4H = OVERFLOW_TOP >> 0x8;
	OCR4C = OVERFLOW_TOP & 0xff;
	TCCR4A = 0;
	TCCR4B = BIT(CS43) | BIT(CS40);
	TCCR4C = BIT(COM4D0) | BIT(PWM4D);
	TCCR4D = 0;

    //  Initialise Timer 1 in normal mode so that it overflows
    //	with a period of approximately 0.5 seconds.
    TCCR1A = 0;
    TCCR1B = 3;
    // Enable timer overflow for Timer 1
    TIMSK1 = 1;
    //	Initialise Timer 0 in normal mode so that it overflows
    //	with a period of approximately 0.008 seconds.
    TCCR0A = 0;
    TCCR0B = 5;
    //	Enable timer overflow interrupt for Timer 0.
    TIMSK0 = 1;
    //	Turn on interrupts.
    sei();
}

// Handles controls 
void setup_inputs()
{
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
}

// setup and initialise game
void start()
{
    set_clock_speed(CPU_8MHz);
    lcd_init(LCD_DEFAULT_CONTRAST);
    adc_init();
    lcd_clear();
    setup_timers();
    setup_inputs();
    D_x = D_y = 0;
    super_J = 0;
    P_x = P_y = 0;
    setup_J();
    setup_T();
    lives = MAX_LIVES;
    pause = false;
    super_J = last_potion = last_firework = last_cheesed = last_trapped = firework_count = cheese_eaten = cheese_count = trap_count = score = counter = minutes = seconds = 0;

    double init_walls[4][4] = {
        {18, 15, 13, 25},
        {25, 35, 25, 45},
        {45, 10, 60, 10},
        {58, 25, 72, 30}};
    for (int i = 0; i < 4; i++)
    {
        walls[i][0] = init_walls[i][0];
        walls[i][1] = init_walls[i][1];
        walls[i][2] = init_walls[i][2];
        walls[i][3] = init_walls[i][3];
    }
    for (int i = 0; i < 5; i++)
    {
        cheeses[i][0] = 0;
        cheeses[i][1] = 0;
    }
    for (int i = 0; i < 5; i++)
    {
        traps[i][0] = 0;
        traps[i][1] = 0;
    }
    for (int i = 0; i < 20; i++)
    {
        fireworks[i][0] = 0;
        fireworks[i][1] = 0;
    }
}

// to run when game is started
void start_screen()
{
    draw_string(5, 10, "Jamie Martin", FG_COLOUR);
    draw_string(5, 20, "n10212361", FG_COLOUR);
    draw_string(5, 30, "T + J", FG_COLOUR);
    if (actions[5][0])
    {
        next_level = true;
    }
}

// To run when game is over
void do_game_over()
{
    draw_string(5, 10, "Game Over", FG_COLOUR);
    draw_string(5, 20, "RB - Restart", FG_COLOUR);
    if (actions[5][0])
    {
        start();
        next_level = true;
    }
}

int main(void)
{
    levels = 3;
    start();
    for (level = 0; level < levels + 1;)
    {
        for (;;)
        {
            clear_screen();

            int r_adc = adc_read(1);
            int l_adc = adc_read(0);
            right_adc = ((double)r_adc / 1024) - 0.5;
            left_adc = (double)l_adc / 1024;

            if ( super_J > 0) {
                if (BIT_IS_SET(PORTB, 2)) {
                    CLEAR_BIT(PORTB, 2);
                } else {
                    SET_BIT(PORTB, 2);
                } 

                if (BIT_IS_SET(PORTB, 3)) {
                    CLEAR_BIT(PORTB, 3);
                } else {
                    SET_BIT(PORTB, 3);
                }
            } else {
                CLEAR_BIT(PORTB, 2);
                CLEAR_BIT(PORTB, 3);
            }

            if (level == 0)
                start_screen();
            else if (level == -1) {
                do_game_over();
                start();
            }
            else if (level >= 1)
                process();
            
            show_screen();

            if (next_level) {
                level++;
                cheese_eaten = 0;
                level2 = false;
            }
            if (lives <= 0 || level == levels) level = -1;

            if (next_level)
            {
                next_level = pause = false;
                break;
            }
        }
    }
}