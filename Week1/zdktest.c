#include <cab202_graphics.h>

int main() {
    setup_screen();
    //the drawing part of your code goes here
    draw_string(10, 20, "Hello World!");
    cleanup_screen();
    return 0;
}
