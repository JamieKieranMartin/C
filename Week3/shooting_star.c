#include <stdlib.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>

bool game_over = false;

//  Declare global variables.

double x, y, dx, dy;
#define STAR '*'
#define BOMB '#'

void setup(void) {
    // Insert setup code here.
    x = 11;
    y = 16;
    dx = 69;
    dy = 7;

    int t1 = dx - x;
    int t2 = dy - y;

    double d = sqrt((t1*t1) + (t2*t2));
    dx = t1 * 0.15 / d;
    dy = t2 * 0.15 / d;

    draw_char(11, 16, BOMB);
    draw_char(69, 7, STAR);
}

void loop(void) {
    // Insert loop code here.
    x = x + dx;
    y = y + dy;
    clear_screen();

    draw_char(69, 7, STAR);
    draw_char(round(x), round(y), BOMB);

    if (round(x) == 69 && round(y) == 7) {
        game_over = true;
    }

    show_screen();
}

int main() {
    // milliseconds sleep between calls to loop.
    const int DELAY = 10;

    setup_screen();
    setup();
    show_screen();

    while (!game_over) {
        loop();
        timer_pause(DELAY);
    }

    return 0;
}
