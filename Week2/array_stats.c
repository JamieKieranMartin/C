#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

//  Declare Global variables here.
double list[29];
int list_size;
double list_mean;
double list_min;
double list_max;

void array_stats() {
    list_size = 0;

    while (list_size < 29 && scanf("%lf", &list[list_size]) == 1) {
        list_size++;
    }
    list_mean = 0;
    list_min = DBL_MAX;
    list_max = DBL_MIN;

    for (int i = 0; i < list_size; i++) {
        if (list[i] < list_min) {
            list_min = list[i];
        }

        if (list[i] > list_max) {
            list_max = list[i];
        }

        list_mean = list_mean + list[i];
    }
    list_mean = list_mean / list_size;

    if (isnan(list_mean)) {
        list_mean = 0;
    }

    if (list_max == DBL_MIN) {
        list_max = 0;
    }

    if (list_min == DBL_MAX) {
        list_min = 0;
    }
}

#include <stdlib.h>
#include <time.h>

int main() {
    // Simulate the test setup process.
    srand( time( NULL ) );
    for ( int i = 0; i < 29; i++ ) {
        list[i] = rand();
    }
    list_size = rand();
    list_mean = rand();
    list_min = rand();
    list_max = rand();

    // Call submitted code.
    array_stats();

    // Display contents of array list.
    for (int i = 0; i < list_size; i++) {
        printf("%f ", list[i]);
    }

    printf("\n");
    printf("Item count: %d\n", list_size);
    printf("Item mean : %f\n", list_mean);
    printf("Item min  : %f\n", list_min);
    printf("Item max  : %f\n", list_max);

    return 0;
}
