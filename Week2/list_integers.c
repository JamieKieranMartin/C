#include <stdio.h>

void list_integers( void ) {
    #define MIN 56
    #define MAX 393
    // Display the title message.
    printf("Counting from %d up to %d...", MIN, MAX);
    //  Enter a counter-controlled loop which will start at 56 and
    //  continues up to and including 393, advancing by increments of 1. 
    for (int i = MIN; i <= MAX; i++) {
        //  Print the current value of the counter variable.
        printf("\n%d", i);
    }
    // End the loop.
}

int main( void ) {
	list_integers( );
	return 0;
}
