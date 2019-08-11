#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>

#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288   /* pi             */
#endif

// Set this to true when the game is over
bool game_over = false;
bool pause = false;

// Jerry's position + state
double J_x = 1, J_y = 1;
int lives;
int score;
#define J_MARKER 'J'
#define MAX_LIVES 5

// Tom's position + state
double T_x, T_y, T_dx, T_dy;
int cheese_count;
int trap_count;
int weapon_count;
#define T_MARKER 'T'

int minutes;
int seconds;
int milliseconds;

// Screen dimensions
int W, H;

void draw_border() {
    const int ch = '*';
    draw_line(0, 0, 0, H - 1, ch);
    draw_line(0, 0, W - 1, 0, ch);
    draw_line(0, 2, W - 1, 2, ch);
    draw_line(0, H - 1, W - 1, H - 1, ch);
    draw_line(W - 1, 0, W - 1, H - 1, ch);
    
    draw_line(0, 3, 20, 20, ch);
}

/**
 *  Draws the status bar and information
 */
void draw_status() {
    draw_formatted(0, 0, "N10212361  Score: %d  Lives: %d  Active: J  Time: %02d:%02d ", score, lives, minutes, seconds);
    draw_formatted(0, 2, "Cheese: %d MouseTraps: %d Weapons: %d", cheese_count, trap_count, weapon_count);
}

/**
 *  Detects collision between objects
 *
 *  double (x0, y0) position of object 1
 *  double (x1, y1) position of object 2
 *
 *  Returns true if the locations are the same
 */
bool collided(double x0, double y0, double x1, double y1) {
    return round(x0) == round(x1) && round(y0) == round(y1);
}

/**
 *	Sets up Jerry, placing initially in the centre of the screen
 */
void setup_J() {
    J_x = (W - 1) / 2;
    J_y = (H - 1) / 2;
}

/**
 *	Draws Jerry Marker
 */
void draw_J() {
    draw_char(round(J_x), round(J_y), J_MARKER);
}

/**
 *	Updates the position of Jerry based on a key press
 *	int ch, the key
 */
void update_J(int ch) {
    if (ch == 'a' && J_x > 1) {
        J_x--;
    }
    else if (ch == 'd' && J_x < W - 2) {
        J_x++;
    }
    else if (ch == 's' && J_y < H - 2) {
        J_y++;
    }
    else if (ch == 'w' && J_y > 3) {
        J_y--;
    }
}

/**
 *	Draws Tom Marker
 */
void draw_T() {
    draw_char(round(T_x), round(T_y), T_MARKER);
}

/**
 *	Sets up Tom at a random location and direction
 */
void setup_T() {
    T_x = 1 + rand() % (W - 2);
    T_y = 3 + rand() % (H - 4);

    double T_dir = rand() * M_PI * 2 / RAND_MAX;
    const double step = 0.1;

    T_dx = step * cos(T_dir);
    T_dy = step * sin(T_dir);
}

void do_collided() {
    clear_screen();

    const char *message[] = {
        "Game over!",
        "Press q to exit..."
    };

    const int rows = 2;

    for (int i = 0; i < rows; i++) {
        // Draw message in middle of screen.
        int len = strlen(message[i]);
        int x = (W - len) / 2;
        int y = (H - rows) / 2 + i;
        draw_formatted(x, y, message[i]);
    }

    show_screen();

    while (get_char() > 0) {}

    game_over = true;
}

/**
 *	Moves Tom a single step (if possible) with reflection
 *	from the border.
 */
void move_T() {
    // Assume that zombie is have not already collided with the borders.
    // Predict the next screen position of the zombie.
    int new_x = round(T_x + T_dx);
    int new_y = round(T_y + T_dy);

    bool bounced = false;

    if (new_x == 0 || new_x == screen_width() - 1) {
        // Bounce of left or right wall: reverse horizontal direction
        T_dx = -T_dx;
        bounced = true;
    }

    if (new_y == 2 || new_y == screen_height() - 1) {
        // Bounce off top or bottom wall: reverse vertical direction
        T_dy = -T_dy;
        bounced = true;
    }

    if (!bounced) {
        // no bounce: move instead.
        T_x += T_dx;
        T_y += T_dy;
    }
}

/**
 *	Moves Tom (if it their turn), and checks for collision
 *	with hero.
 */
void update_T(int key) {
    if (key < 0) {
        move_T();
    }

    if (collided(J_x, J_y, T_x, T_y)) {
        lives--;
        setup_J();
        setup_T();
        

        if (lives == 0) {
            do_collided();
        }
    }
}

void draw_all() {
    clear_screen();
    draw_border();
    draw_status();
    draw_J();
    draw_T();
    show_screen();
}

void timer_manage(int DELAY) {
    milliseconds = milliseconds + DELAY;

    if (milliseconds > 1000) {
        seconds++;
        milliseconds = 0;
    }

    if (seconds >= 60) {
        minutes++;
        seconds = 0;
    }
}

// Insert other functions here, or include header files
void setup() {
    cheese_count = 0, trap_count = 0, weapon_count = 0;
    lives = MAX_LIVES;
    score = 0;
    srand(get_current_time());
    W = screen_width();
    H = screen_height();

    setup_J();
    setup_T();
}
void loop() {
    // Insert loop logic here.
    int key = get_char();

    if (key == 'q') {
        game_over = true;
        return;
    }

    if (key == 'p') {
        pause = !pause;
        return;
    }

    update_J(key);
    update_T(key);
}

int main() {
    const int DELAY = 10;
    seconds=milliseconds=0;

    setup_screen();
    setup();

    while (!game_over) {
        draw_all();
        loop();
        timer_pause(DELAY);
        if (!pause) timer_manage(DELAY);
    }
    return 0;
}