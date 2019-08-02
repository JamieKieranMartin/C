#include <stdio.h>

void count_down( void ) {
    //  Insert your solution here
    #define MAX 3237
    #define MIN 93
    #define STEP 7

    for (int i = MAX; i >= MIN; i=i-STEP) {
        printf("%d\n", i);
    }
}

int main( void ) {
	count_down( );
	return 0;
}
