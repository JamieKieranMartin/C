#include <stdio.h>

void cab_202( void ) {
    // Insert your solution here.
    #define MAX 3557
    #define MIN 23
    
    for (int i = MIN; i <= MAX; i++) {
        if ((i % 7 == 0) && (i % 19 != 0)) {
            printf("CAB");
        } else if ((i % 19 == 0) && (i % 7 != 0)) {
            printf("202");
        } else if ((i % 7 == 0) && (i % 19 == 0)) {
            printf("CAB202");
        } else {
            printf("%d", i);
        }
        printf("\n");
    }

}

int main( void ) {
	cab_202( );
	return 0;
}