#include <stdlib.h>
#include <cab202_graphics.h>

 void fill_rect(
     int leftmost,
     int upper,
     int rightmost,
     int lower,
     char display_char
 ) {
    //  Test to see if either the width or height of the rectangle is less 
    //  than 1. If so, the function should stop immediately and draw nothing.
    if (leftmost > rightmost || upper > lower) {
        return;
    } else {
        for (int i = upper; i <= lower; i++) {
            draw_line(leftmost, i, rightmost, i, display_char);
        }
    }
}

int main( void ) {
	int l, t, r, b;
	char c;

	printf( "Please enter the horizontal location of the left edge of the rectangle: " );
	scanf( "%d", &l );

	printf( "Please enter the vertical location of the top edge of the rectangle: " );
	scanf( "%d", &t );

	printf( "Please enter the horizontal location of the right edge of the rectangle: " );
	scanf( "%d", &r );

	printf( "Please enter the vertical location of the bottom edge of the rectangle: " );
	scanf( "%d", &b );

	printf( "Please enter the character used to draw the rectangle? " );
	scanf( " %c", &c );

	setup_screen();
	fill_rect( l, t, r, b, c );
	show_screen();
	wait_char();

	return 0;
}
