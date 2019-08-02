#include <stdio.h>
#include <stdlib.h>

//  Declare an array called rolls with capacity for 91 elements
//  of the default signed integer type.
#define MAX 91
int rolls[MAX];

void roll_dice( void ) {
    //  Seed the random number generator with the value 104452.
    srand(104452);
    //  Generate 91 random values between 1 and 9 inclusive. To do 
    //  this, use a counter-controlled loop which will visit every element of 
    //  rolls. The counter should start at zero, and the loop should continue 
    //  while the counter is less than 91.
    for (int i = 0; i < MAX; i++) {
        // Generate a random value between 1 and 9 inclusive. Store this 
        //  value in the current element of rolls - that is, the element 
        //  indexed by the loop counter.
        rolls[i] = (rand() % 9) + 1;
    }
    //  End the loop.
}

int main(void) {
    roll_dice();

    // Display contents of array rolls.
    for (int i = 0; i < 91; i++) {
        if (i > 0) {
            printf(",");
        }
        printf("%d", rolls[i]);
    }

    printf("\n");

    return 0;
}
