#include <stdio.h>
#include <stdbool.h>

//  (Task 1) Define a function called position_read which uses printf and 
//  scanf to get the user to enter an ordered pair of the default signed 
//  integer data type, and store the ordered pair in a pair of integer 
//  variables provided by the caller.
//
//  Parameters:
//      pos_1 - the address of a variable where the first of two integer 
//          values will be stored by the function. You will need to use pointer 
//          notation to express this idea.
//
//      pos_2 - the address of a variable where the second of two integer 
//          values will be stored by the function. You will need to use pointer 
//          notation to express this idea.
//
//  Returns:
//      The function returns a boolean value indicating the status of the 
//      I/O operation. The status is true if and only if two integer values 
//      have been successfully parsed.

bool position_read ( int *pos_1, int *pos_2 )
{   
    //  (Task 1.1) Display prompt:
    //  "Please enter two integer values, separated by spaces:".
    printf("Please enter two integer values, separated by spaces:");
    scanf("%d %d", pos_1, pos_2);
    printf("%d %d ===", *pos_1, *pos_2);
    //  (Task 1.2) Use scanf: try to read a pair of integer values into the
    //  first two elements of pos. Save the value returned by scanf in a
    //  local variable (which might be called items_read).
    int items_read[2];
    items_read[0] = *pos_1;
    items_read[1] = *pos_2;
    printf("%d %d\n", items_read[0] + items_read[1]);
    //  (Task 1.3) Return true if and only if items_read is equal to 2.
    if (items_read[0] + items_read[1] == 2) {
        return true;
    } else {
        return false;
    }
}

//  (Task 2) Define a function called position_write which uses printf to 
//  send the value of an ordered pair of integers to the standard output 
//  stream, separated by a space. 
// 
//  Parameters:
//      pos_1 - an integer containing the first value in the pair.
//
//      pos_2 - an integer containing the second value in the pair.
//
//  Returns:
//      Nothing.

void position_write ( int pos_1, int pos_2 )
{
    //  (Task 2.1) Send the ordered pair to standard output using printf with
    //  with format string "{%d, %d}". Do NOT insert a linefeed.
    printf("{%d, %d}", pos_1, pos_2);
}

//  (Task 3) Define a function called position_dist which calculates the city 
//  block distance between two ordered pairs of integers, x=(x_1,x_2)
//  and y=(y_1,y_2).
//  Ref: https://en.wikipedia.org/wiki/Taxicab_geometry
// 
//  Parameters:
//      x_1 - an integer containing the first element of the first ordered pair.
//      x_2 - an integer containing the second element of the first ordered pair.
//
//      y_1 - an integer containing the first element of the second ordered pair.
//      y_2 - an integer containing the second element of the second ordered pair.
//
//  Returns:
//      An int which is equal to the city block distance between the points
//      represented by the arguments.

int position_dist ( int x_1, int x_2, int y_1, int y_2 )
{
    //  (Task 3.1) Compute and return the city block distance between the points 
    //  represented by ADT_TYPE_pos1 and ADT_TYPE_pos2.
    int pos_1 = x_1 - y_1;
    int pos_2 = x_2 - y_2;
    if (pos_1 < 0) {
        pos_1 = pos_1 * -1;
    }
    if (pos_2 < 0) {
        pos_2 = pos_2 * -1;
    }
    return pos_1 + pos_2;
}

//  (Task 4) Define a function called position_nearest which finds the
//  ADT_TYPE object in a list which is closest to a query point, using
//  city block distance. The query point is represented by a pair of integer 
//  variables The list is represented by a pair of parallel int arrays.
// 
//  Parameters:
//      query_pos_1 - The first coordinate of the query point.
//  
//      query_pos_2 - The second coordinate of the query point.
//
//      positions_1 - An array of integers containing the first coordinates of the
//          list that will be searched.
//
//      positions_2 - An array of integers containing the second coordinates of the
//          list that will be searched.
//
//      num_positions - an int which tells the function how many ordered pairs there 
//      are in the list.
//
//  Returns:
//      The index of the nearest point in the list, unless the list is empty, 
//      the value -1 will be returned. The list is empty if and only if 
//      num_positions <= 0.
//
//      If two or more points share the minimum distance, then the lowest of 
//      the corresponding indices will be returned.

int position_nearest ( int query_pos_1, int query_pos_2, int positions_1[], int positions_2[], int num_positions )
{
    //  (Task 4.1) If num_positions equal to or less than 0, return -1.
    if (num_positions <= 0) {
        return -1;
    }
    //  (Task 4.2) Declare and initialise an integer called nearest.
    //  The initial value is the position of the first element in an array.
    int nearest = 0;
    //  (Task 4.3) Declare and initialise an integer called min_dist.
    //  The initial value is the city block distance from the query to
    //  the first pair in the list.
    //  Hint: use position_dist to find the distance between (query_pos_1, query_pos_2)
    //  and (positions_1[0], positions_2[0]).
    int min_dist = position_dist (query_pos_1, query_pos_2, positions_1[0], positions_2[1]);
    //  (Task 4.4) Set up a for loop to iterate over the list.
    //  Skip the first pair in the list, because we already know the distance 
    //  from the first pair to the query. The counter will act as an index into 
    //  the list.
    for ( int i = 1; i < num_positions; i++ )
    {
        //  (Task 4.4.1) Compute the city block distance from the query
        //  to the current pair in the list. This is the current distance. 
        //  Store the current distance in a new local variable.
        //  Hint: Distance between (query_pos_1, query_pos_2) and (positions_1[i], positions_2[i]).
        int curr_dist = position_dist(query_pos_1, query_pos_2, positions_1[i], positions_2[i]);
        //  (Task 4.4.2) If the current distance is less than the minimum distance:
        if ( curr_dist < min_dist )
        {
            //  (Task 4.4.3) The minimum distance is overwritten by the current
            //  distance and the nearest index is overwritten by the current index.
            min_dist = curr_dist;
            nearest = i;
        }
    }

    //  (Task 4.5) Return nearest.
    return nearest;
}


#define MAX_ITEMS (100)

int main(void)
{
    int q_1, q_2;
    printf("Input query point:\n");
    position_read(&q_1, &q_2);

    int x_1[MAX_ITEMS];
    int x_2[MAX_ITEMS];
    int num_items;

    // Get number of ref_points.
    printf("Please enter number of reference points (up to %d) that will be processed: ", MAX_ITEMS);
    scanf("%d", &num_items);

    // if number of ref_points exceeds array size, restrict it to that value.
    if (num_items > MAX_ITEMS)
    {
        num_items = MAX_ITEMS;
    }

    for (int i = 0; i < num_items; i++)
    {
        printf("Please enter item %d of %d:\n", (i + 1), num_items);
        position_read(&x_1[i], &x_2[i]);
    }

    for (int i = 0; i < num_items; i++)
    {
        printf("City block distance from ");
        position_write(x_1[i], x_2[i]);
        printf(" to ");
        position_write(q_1, q_2);
        printf(" is %d\n", position_dist(x_1[i], x_2[i], q_1, q_2));
    }

    int nearest = position_nearest(q_1, q_2, x_1, x_2, num_items);

    if (nearest >= 0)
    {
        int p_1 = x_1[nearest];
        int p_2 = x_2[nearest];
        int nearest_dist = position_dist(q_1, q_2, p_1, p_2);

        printf("The closest point is ");
        position_write(p_1, p_2);
        printf(", at a distance of %d.\n", nearest_dist);
    }
    else
    {
        printf("Collection is empty.\n");
    }

    return 0;
}