#include "cab202_graphics.h"

int main() {
    setup_screen();
    
    draw_string(0, 0, "Hello World!");
    show_screen();
    while(getchar());
    cleanup_screen();
    return 0;
}
