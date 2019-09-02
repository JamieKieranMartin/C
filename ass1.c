#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>

// Set this to true when the game is over
bool pause = false, next_level = false;
bool game_won = false, game_quit = false, game_over = false;

// Jerry's position + state
double J_x, J_y, sJ_x = 1, sJ_y = 1;
int lives, score, levels, level = 1;
int F_x = -1, F_y = -1;
#define J_MARKER 'J'
#define MAX_LIVES 5

// Tom's position + state
double T_x, T_y, sT_x = 1, sT_y = 1;
#define T_MARKER 'T'

// Item + Object globals
int walls[50][4];
int wall_count;
#define WALL '*'
int cheese_count, trap_count, weapon_count;
int cheeses[5][2];
#define CHEESE 'C'
int traps[5][2];
#define TRAP '#'
int D_x, D_y;
#define DOOR 'X'

// Message constants
const char *game_over_message[] = {
        "Game over!",
        "Press q to exit..."
};

const char *game_won_message[] = {
        "Game won!",
        "Press q to exit..."
};

// Timer globals
int minutes, seconds, milliseconds, total_seconds, last_cheesed, last_trapped;
// Screen dimensions
int W, H;

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
 * Generates a random position in the field of play
 */
int random_position(char type) {
    if (type == 'y') {
        return (rand() % (H - 5)) + 4;
    } else if (type == 'x') {
        return rand() % (W - 1);
    } else return -1;
}

/**
 *	To run when game is done
 */
void do_game(const char *message[], int rows) {
    clear_screen();
    for (int i = 0; i < rows; i++) {
        // Draw message in middle of screen.
        int len = strlen(message[i]);
        int x = (W - len) / 2;
        int y = (H - rows) / 2 + i;
        draw_formatted(x, y, message[i]);
    }
    show_screen();
}

/**
 * To run when game is over
 */
void do_game_over() {
    do_game(game_over_message, 2);
    while (get_char() != 'q') {}
}

/**
 * To run when game is won
 */
void do_game_won() {
    do_game(game_won_message, 2);
    while(get_char() != 'q') {}
}

/**
 *  Draws the status bar and information
 */
void draw_status() {
    draw_formatted(0, 0, "N10212361\tScore:%d\tLives:%d\tPlayer:J\tTime:%02d:%02d\t", score, lives, minutes, seconds);
    draw_formatted(0, 2, "Cheese:%d\tTraps:%d\tFireworks:%d\tLevel:%d", cheese_count, trap_count, weapon_count, level);
    draw_line(0, 3, W, 3, '-');
}

/**
 *	Sets up Jerry, placing initially in the centre of the screen
 */
void setup_J() {
    J_x = sJ_x, J_y = sJ_y;
}

/**
 *	Updates the position of Jerry based on a key press
 *
 *	int ch, the key
 */
void update_J(int ch) {
    // if attempt left & not hitting screen boundary or wall to the left
    if (ch == 'a' && J_x > 0 && scrape_char(J_x - 1, J_y) != WALL)              J_x--;
    // if attempt right & not hitting screen boundary or wall to the right
    else if (ch == 'd' && J_x < W - 1 && scrape_char(J_x + 1, J_y) != WALL)     J_x++;
    // if attempt down & not hitting screen boundary or wall down
    else if (ch == 's' && J_y < H - 1 && scrape_char(J_x, J_y + 1) != WALL)     J_y++;
    // if attempt up & not hitting screen boundary or wall above
    else if (ch == 'w' && J_y > 4 && scrape_char(J_x, J_y - 1) != WALL)         J_y--;

    // loop through cheeses + traps to check if in same position as J
    for (int i = 0; i < 5; i++) {
        if (cheeses[i][0] == J_x && cheeses[i][1] == J_y) {
            cheeses[i][0] = 0, cheeses[i][1] = 0;
            score++, cheese_count--;
            break;
        }

        if (traps[i][0] == J_x && traps[i][1] == J_y) {
            traps[i][0] = 0, traps[i][1] = 0;
            lives--, trap_count--;
            break;
        }
    }
    // if J in position of door
    if (scrape_char(J_x, J_y) == DOOR) next_level = true;
}

/**
 *	Sets up Tom at a random location and direction
 */
void setup_T() {
    T_x = sT_x, T_y = sT_y;
}

/**
 *	Moves Tom a single step (if possible) with reflection
 *	from the border.
 */
void move_T() {
    // get the x and y difference between T + J
    int x_diff = T_x - J_x, y_diff = T_y - J_y;

    // only runs every 200 milliseconds to give player time to react
    if (milliseconds == 200 || milliseconds == 400 || milliseconds == 600 || milliseconds == 800 || milliseconds == 1000) {
        // if the difference is less than 0, meaning T x is to the left of J
        if (x_diff < 0) {
            // only move right if the new x is within the border and not a wall 
            if (T_x + 1 < W && scrape_char(T_x + 1, T_y) != WALL) T_x++;
        }
        // if the difference is greater than 0, meaning T is to the right of J
        else if (x_diff > 0) {
            // only move left if the new x is within the border and not a wall
            if (T_x - 1 > 0 && scrape_char(T_x - 1, T_y) != WALL) T_x--;
        }
        // if the difference is less than 0, meaning T is above J
        if (y_diff < 0) {
            // only move down if the new x is within the border and not a wall
            if (T_y + 1 < W && scrape_char(T_x, T_y + 1) != WALL) T_y++;
        }
        // if the difference is greater than 0, meaning T is below J
        else if (y_diff > 0) {
            // only move up if the new x is within the border and not a wall
            if (T_y - 1 > 0 && scrape_char(T_x, T_y - 1) != WALL) T_y--;
        }
    }
}

/**
 *	Moves Tom (if it their turn), and checks for collision
 *	with Jerry.
 */
void update_T(int key) {
    if (key < 0 && !pause) move_T();

    // if collided reset round
    if (collided(J_x, J_y, T_x, T_y)) {
        lives--;
        setup_J(), setup_T();
        if (lives <= 0) game_over = true;
    }
}

/**
 * Moves the Firework provided it is active and checks for collision with Tom
 */
void update_F() {
    if (F_x != -1 && F_y != -1) {
        weapon_count = 1;
        int x_diff = F_x - T_x, y_diff = F_y - T_y;

        // only runs every 200 milliseconds to give player time to react
        if (milliseconds == 200 || milliseconds == 400 || milliseconds == 600 || milliseconds == 800 || milliseconds == 1000) {
            // if the difference is less than 0, meaning F is to the left of T
            if (x_diff < 0) {
                // only move right if the new x is not a wall, otherwise destory F
                if (scrape_char(F_x + 1, F_y) == WALL)      F_x = F_y = -1;
                else F_x++;
            }
            // if the difference is less than 0, meaning F is to the right of T
            else if (x_diff > 0) {
                // only move left if the new x is not a wall, otherwise destory F
                if (scrape_char(F_x - 1, F_y) == WALL)       F_x = F_y = -1;
                else F_x--;
            }
            // if the difference is less than 0, meaning F is below T
            if (y_diff < 0) {
                // only move down if the new x is not a wall, otherwise destory F
                if (scrape_char(F_x, F_y + 1) == WALL)      F_x = F_y = -1;
                else F_y++;
            } 
            // if the difference is less than 0, meaning F is above T
            else if (y_diff > 0) {
                // only move up if the new x is not a wall, otherwise destory F
                if (scrape_char(F_x, F_y - 1) == WALL)      F_x = F_y = -1;
                else F_y--;
            }
        }

        // if T + F collided reset T and F, add score
        if (F_x == T_x && F_y == T_y) {
            F_x = F_y = -1;
            setup_T();
            score++;
        }
    } else {
        weapon_count = 0;
    }
}

/**
 * Draws Firework 
 */
void draw_F() {
    if (F_x != -1 && F_y != -1) draw_char(F_x, F_y, 'F');
}

/**
 *	Draws all the cheeses in the cheese array
 */
void draw_cheeses() {
    for (int i = 0; i < 5; i++) {
        if (cheeses[i][0] != 0 && cheeses[i][1] != 0) draw_char(cheeses[i][0], cheeses[i][1], CHEESE);
    }
}

/**
 *	Tries to drop a cheese once the time is right, provided there aren't already 5 cheeses
 */
void try_drop_cheese() {
    if (cheese_count < 5 && total_seconds - last_cheesed >= 2) {
        cheese_count++;
        last_cheesed = total_seconds;
        // loops through cheeses and generates random position
        for (int i = 0; i < 5; i++) {
            if (cheeses[i][0] == 0 && cheeses[i][1] == 0) {
                int new_x = random_position('x'), new_y = random_position('y');
                while (scrape_char(new_x, new_y) != ' ') new_x = random_position('x'), new_y = random_position('y');
                cheeses[i][0] = new_x, cheeses[i][1] = new_y;
                break;
            }
        }
    }
}

/**
 *	Draws all the traps in the trap array
 */
void draw_traps() {
    for (int i = 0; i < 5; i++) {
        if (traps[i][0] != 0 && traps[i][1] != 0) draw_char(traps[i][0], traps[i][1], TRAP);
    }
}

/**
 *	Tries to drop a mousetrap once the time is right, provided there aren't already 5 traps
 */
void try_drop_trap() {
    if (trap_count < 5 && total_seconds - last_trapped >= 3) {
        int TomX = T_x, TomY = T_y;
        // loops through traps and generates random position
        for (int i = 0; i < 5; i++) {
            if (traps[i][0] == 0 && traps[i][1] == 0 && (scrape_char(TomX, TomY) == ' ' || scrape_char(TomX, TomY) == T_MARKER)) {
                trap_count++;
                last_trapped = total_seconds;
                traps[i][0] = TomX, traps[i][1] = TomY;
                break;
            }
        }
    }
}

/**
 *	Try open the door for the next level
 */
void try_drop_door() {
    if (score / level == 5 && D_x == 0 && D_y == 0) {
        int new_x = random_position('x'), new_y = random_position('y');
        while (scrape_char(new_x, new_y) != ' ') new_x = random_position('x'), new_y = random_position('y');
        D_x = round(new_x), D_y = round(new_y);
    }
}

/**
 *	Draw the door for each loop
 */
void draw_door() {
    if (D_x != 0 && D_y != 0) draw_char(D_x, D_y, DOOR);
}

/**
 *	Draw walls for each loop
 */
void draw_walls() {
    for (int i = 0; i < wall_count; i++) draw_line(walls[i][0], walls[i][1], walls[i][2], walls[i][3], WALL);
}

/**
 *  Draws all objects on screen
 */
void draw_all(char *argv[]) {
    clear_screen();
    draw_status();
    draw_walls(), draw_cheeses(), draw_traps(), draw_door();
    draw_char(round(T_x), round(T_y), T_MARKER); // Draws Tom
    draw_char(round(J_x), round(J_y), J_MARKER); // Draws Jerry
    draw_F();
    show_screen();
}

/**
 * Manages + updates timer
 */
void timer_manage(int DELAY) {
    milliseconds = milliseconds + DELAY;
    // update seconds when milliseconds hits 1000
    if (milliseconds > 1000) {
        seconds++;
        total_seconds++;
        milliseconds = 0;
    }
    // update minutes when seconds hits 60
    if (seconds >= 60) {
        minutes++;
        seconds = 0;
    }
}

/**
 *	Read the level file and store data into variables
 */
void prep_level (FILE *stream) {
    while(!feof(stream)) {
        char command;
        double arg1, arg2, arg3, arg4;
        fscanf(stream, "%c%lf%lf%lf%lf", &command, &arg1, &arg2, &arg3, &arg4);
        arg1 = arg1 * (W - 1);
        arg2 = arg2 * (H - 5) + 4;
        arg3 = arg3 * (W - 1);
        arg4 = arg4 * (H - 5) + 4;

        // set J start
        if (command == 'J') sJ_x = round(arg1), sJ_y = round(arg2);
        // set T start
        else if (command == 'T') sT_x = round(arg1), sT_y = round(arg2);
        // set Wall Coords
        else if (command == 'W') {
            walls[wall_count][0] = round(arg1), walls[wall_count][1] = round(arg2);
            walls[wall_count][2] = round(arg3), walls[wall_count][3] = round(arg4);
            wall_count++;
        }
    }
}

/**
 * Setup Game to Start
 */
void setup(char *argv[]) {
    // resets values
    minutes = seconds = milliseconds = total_seconds = last_cheesed = last_trapped = 0;
    weapon_count = 0;
    F_x = F_y = -1;
    D_x = D_y = 0;

    // resets walls, traps, cheese to 0
    for (int i = 0; i < wall_count; i++) walls[i][0] = 0, walls[i][1] = 0, walls[i][2] = 0, walls[i][3] = 0;
    for (int i = 0; i < trap_count; i++) traps[i][0] = 0, traps[i][1] = 0;        
    for (int i = 0; i < cheese_count; i++) cheeses[i][0] = 0, cheeses[i][1] = 0;

    cheese_count = trap_count = wall_count = 0;
    
    // opens next file
    FILE *stream = fopen(argv[level], "r");
    if (stream != NULL)
    {
        prep_level(stream);
        fclose(stream);
    }

    // sets up characters
    setup_J(), setup_T();
}

/**
 *	To run each loop
 */
void loop() {
    // Insert loop logic here.
    int key = get_char();

    // if press q then prime game_quit to true
    if (key == 'q') {
        game_quit = true;
        return;
    }

    // if press q then pause T, Timer, etc
    if (key == 'p') {
        pause = !pause;
        return;
    }

    // if press f then prime Firework
    if (key == 'f') {
        F_x = J_x, F_y = J_y;
        weapon_count++;
        return;
    }

    // if press l then prime next level
    if (key == 'l') {
        next_level = true;
        return;
    }

    // update characters + objects
    update_J(key);
    update_F();
    update_T(key);
    try_drop_door();
    if (!pause) {
        try_drop_cheese();
        try_drop_trap();
    }
}

/**
 * Main Program
 */
int main(int argc, char *argv[]) {
    const int DELAY = 10;
    setup_screen();
    srand(get_current_time());
    W = screen_width(), H = screen_height();
    lives = MAX_LIVES;
    score = 0;
    levels = argc;

    // level loop
    for (; level < levels; level++) {
        setup(argv);

        while (1) {
            draw_all(argv);
            loop();
            timer_pause(DELAY);
            if (!pause) timer_manage(DELAY);
            if (next_level || game_over || game_won || game_quit) break;
        }
        if (game_over || game_won || game_quit) break;
        next_level = false;
        clear_screen();
    }

    // end of game messages
    if (level == levels) game_won = true;
    if (game_won) do_game_won();
    else if (game_over) do_game_over();
    
    return 0;
}