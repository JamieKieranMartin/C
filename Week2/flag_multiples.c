#include <stdio.h>

void flag_multiples( void ) {
    #define MAX 3557
    #define MIN 23
    #define FACTOR 7
    //  Enter a counter controlled loop which starts at 23 and
    //  advances by increments of 1 up to and including 3557
    for (int i = MIN; i <= MAX; i++) {
        //  Send the current value of the counter to standard output as a 
        //  decimal formatted integer.
        printf("%d", i);
        //  If the current value of the counter is divisible by 7 then 
        //  highlight the current line by sending the pattern " <===" to 
        //  standard output.
        if (i % FACTOR == 0) {
            printf(" <===");
        }
        //  Send a linefeed to standard output.
        printf("\n");
    }
    //  End loop.
}

int main( void ) {
	flag_multiples( );
	return 0;
}
