#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

//  (Task 1) Define a function called date_read which uses scanf to get the 
//  data for a date. Fields are to be entered as three separate int values
//  in the format "%d/%d/%d". Dates will be entered with day first, followed by
//  month, followed by year.
// 
//  Parameters:
//      d - the address of an integer which will represent the day.
//      m - the address of an integer which will represent the month.
//      y - the address of an integer which will represent the year.
//
//  Returns:
//      The function must return a boolean value indicating the status of the 
//      I/O operation. The result is true if and only if three integer values 
//      have been successfully parsed using the designated format string.
//
//      Do not try to perform other data validation in this function.

bool date_read(int *d, int *m, int *y)
{
    if (scanf("%d/%d/%d", d, m, y) == 3) {
        return true;
    } else {
        return false;
    }
}

//  (Task 2) Define a function called date_write which uses printf to 
//  display the value of a date in format "%d/%d/%d", with day followed by month
//  followed by year.
// 
//  Parameters:
//      d - an integer which will represent the day.
//      m - an integer which will represent the month.
//      y - an integer which will represent the year.
//
//  Returns:
//      Nothing.

void date_write(int d, int m, int y)
{
    printf("%d/%d/%d", d, m, y);
}

//  (Task 3) Define a function called date_compare which compares two
//  dates. Your implementation may assume that these values are valid dates.
// 
//  Parameters:
//      d1, m1, y1 - Three integers representing the first date. 
//      d2, m2, y2 - Three integers representing the second date. 
//
//  Returns:
//      An int which is:
//      -1  if the date represented by (d1, m1, y1) is before that 
//          represented by (d2, m2, y2);
//      0   if the two values represent the same date;
//      +1  otherwise.

int date_compare(int d1, int m1, int y1, int d2, int m2, int y2)
{
    if (y1 < y2) return -1;
    else if (y1 > y2) return 1;
    else if (y1 == y2) {
        if (m1 < m2) return -1;
        else if (m1 > m2) return 1;
        else if (d1 < d2) return -1;
        else if (d1 > d2) return 1;
        else return 0;
    } else return 0;
}

//  (Task 4) Implement the date_valid function which determines if the 
//  supplied date is valid:
//  *   Year must be greater than or equal to 1.
//  *   Month must between 1 and 12 inclusive.
//  *   Day must be at least 1, with upper limits given below:
//      30 days: September, April June, and November.
//      31 days: January, March, May, July, August, October, December.
//      28 or 29: February (usually 28, but 29 in a leap year).
//  
//  A year is a leap year if it is divisible by 400, or if it is
//  divisible by 4 but not divisible by 100.
//  
//  Parameters:
//     d, m, y - three integers representing a date.
//
//  Returns:
//      Returns true if and only if the supplied date is valid according to 
//      the definition listed above.

bool date_valid(int d, int m, int y)
{
    if (1 <= y && 1 <= m && m <= 12 && 1 <= d && d <= 31) {
        if ((m == 4 || m == 6 || m == 9 || m == 11) && d <= 30) {
            return true;
        } else if ((m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12) && d <= 31) {
            return true;
        } else if (m == 2 && (y % 400 == 0 || (y % 4 == 0 && y % 100 != 0)) && d <= 29) {
            return true;
        } else if (m == 2 && d <= 28) {
            return true;
        } else return false;
    } else return false;
}

//  (Task 5) Define a function called date_match which compares a query date to
//  a list of dates. The function returns the position of the first object in 
//  the list which satisfies a designated criterion.
//
//  If criterion is -1, then the function returns the position of 
//  the first date in the list that comes before the query date. If criterion is
//  +1, then the function returns the position of the first date in the list 
//  that comes after the query date. If criterion is 0, then the function returns 
//  the position of the first date in the list that is equal to the query.
// 
//  Parameters:
//      d, m, y    - Three integers which represents a query date.
//
//      dd, mm, yy - Three integer arrays, holding respective days, months and 
//                   years of a list of dates.
//
//      num_dates  - an int which tells the function how many elements there 
//                   are in the array.
//
//      criterion  - an int (guaranteed to be -1, 0, or 1) which defines the 
//                   matching criterion. 
//
//  Returns:
//      The integer position of the first item in the list which satisfies the 
//      matching criterion.
//
//      If num_items is equal to or less than 0, the function returns -1.
//
//      If the query is not a valid date, the function returns -1.
//
//      If there is integer i for which 
//          0 <= i < num_dates,  
//      and
//          date_valid( dd[i], mm[i], yy[i] )
//      and
//          date_compare( dd[i], mm[i], yy[i], d, m, y ) == criterion
//      then 
//          the function returns i.
//
//      Otherwise the function returns -1.

int date_match(int d, int m, int y, int dd[], int mm[], int yy[], int num_dates, int criterion)
{
    if (num_dates <= 0) {
        return -1;
    } else if (date_valid(d, m, y)) {
        int res;
        for (int i = 0; i < num_dates; i++) {
            if (date_valid(dd[i], mm[i], yy[i])) {
                res = date_compare(dd[i], mm[i], yy[i], d, m, y);

                if (criterion == -1 && res == -1) {
                    return i;
                } else if (criterion == 1 && res == 1) {
                    return i;
                } else if (criterion == 0 && res == 0) {
                    return i;
                }

            }
        }
    }
    
    return -1;
}


#define MAX_ITEMS (100)

int main(void)
{
    int query_day, query_month, query_year;
    printf("Input query date using format %s: ", "%d/%d/%d");
    date_read(&query_day, &query_month, &query_year);

    int dd[MAX_ITEMS], mm[MAX_ITEMS], yy[MAX_ITEMS];
    int num_items;

    // Get number of ref_dates.
    printf("Please enter number of items (up to %d) that will be processed: ", MAX_ITEMS);
    scanf("%d", &num_items);

    // if number of ref_dates exceeds array size, restrict it to that value.
    if (num_items > MAX_ITEMS)
    {
        num_items = MAX_ITEMS;
    }

    for (int i = 0; i < num_items; i++)
    {
        printf("Please enter item %d of %d using format %s, with day first and year last: ", (i + 1), num_items, "%d/%d/%d");
        bool ok = date_read(&dd[i], &mm[i], &yy[i]);

        if ( ! ok ) {
            num_items = i;
            break;
        }
    }

    for (int i = 0; i < num_items; i++)
    {
        date_write(dd[i], mm[i], yy[i]);

        if (!date_valid(dd[i], mm[i], yy[i]))
        {
            printf(" is not valid.\n");
            continue;
        }

        int cmp = date_compare(dd[i], mm[i], yy[i], query_day, query_month, query_year);

        if (cmp < -1 || cmp > 1)
        {
            printf("Error!!! date_compare is broken.\n");
            exit(1);
        }

        char * labels[] = { 
            "less than", 
            "equal to", 
            "greater than" 
        };

        printf(" is %s ", labels[cmp + 1]);
        date_write(query_day, query_month, query_year);
        printf("\n");
    }

    const int criterion = -1;

    int match_idx = date_match(
        query_day, query_month, query_year, 
        dd, mm, yy, 
        num_items, criterion
    );

    if (match_idx >= 0)
    {
        printf("The first valid date matching the search criterion is ");
        date_write(dd[match_idx], mm[match_idx], yy[match_idx]);
    }
    else
    {
        printf("There is no valid date matching the search criterion.\n");
    }

    return 0;
}
