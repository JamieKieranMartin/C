#include <stdio.h>
#include <stdlib.h>

//  Declare an array called fib with capacity for 27 elements
//  of the default signed integer type.
int fib[27];

void fibonacci( void ) {
    //  Insert your solution here.
    int X, Y;
    scanf("%d", &X);
    scanf("%d", &Y);
    fib[0] = X;
    fib[1] = Y;

    for (int i = 2; i < 27; i++) {
        fib[i] = fib[i-1] + fib[i-2];
    }
}

int main(void) {
    fibonacci();

    // Display contents of array fib.
    for (int i = 0; i < 27; i++) {
        printf("%d\n", fib[i]);
    }

    return 0;
}
