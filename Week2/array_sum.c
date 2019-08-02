#include <stdio.h>
#include <stdlib.h>

//  Declare an array called seq with capacity for 29 double precision 
//  floating point values.
double seq[29];
//  Declare an integer called seq_len which will track the number of items
//  that have been appended to the list stored.
int seq_len;
//  Declare a double precision floating point value called seq_sum which will 
//  be used to remember the total sum of the items stored in the list.
double seq_sum;

void array_sum() {
    //  Initially the list is empty, so assign 0 to seq_len.
    seq_len = 0;
    //  Append items to the list by reading from standard input until either
    //  seq_len >= 29, or the value returned by scanf is not 1.
    while (seq_len < 29 && scanf("%lf", &seq[seq_len]) == 1) {
        seq_len++;
    }
    //  Assign 0 to seq_sum.
    seq_sum = 0;
    //  Traverse the items that have been appended to the list, and accumulate 
    //  the sum in seq_sum.
    for (int i = 0; i < seq_len; i++) {
        seq_sum = seq_sum + seq[i];
    }
}
#include <stdlib.h>
#include <time.h>

int main(void) {
    // Simulate the test setup process.
    srand( time( NULL ) );
    for ( int i = 0; i < 29; i++ ) {
        seq[i] = rand();
    }
    seq_len = rand();
    seq_sum = rand();

    // Call submitted code.
    array_sum();

    // Display contents of array seq.
    for (int i = 0; i < seq_len; i++) {
        printf("%f ", seq[i]);
    }

    printf("\n");
    printf("Item count: %d\n", seq_len);
    printf("Item sum  : %f\n", seq_sum);

    return 0;
}
