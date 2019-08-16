#include <stdio.h>
#include <stdbool.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>

/**
 *  Define a function called show_file which parses a list of drawing
 *  commands from a text file, rendering them to the ZDK screen.
 *
 *  Parameters:
 *      stream - The address of a FILE object which provides access to the stream.
 *
 *  Returns:
 *      nothing.
 */

void show_file (FILE *stream)
{
    //  Declare a variable called pen of type char, with initial value '*'.
    char pen = '*';

    //  While stream has not reach end of input:
    while(!feof(stream))
    {
        //  Declare a variable called command of type char.
        char command;
        //  Declare four variables of type int, with names arg1, arg2, arg3, arg4.
        int arg1, arg2, arg3, arg4;
        //  Use a single call to fscanf to (attempt to) skip leading spaces. 
        //  To do this, read a char and four int values into command, arg1, arg2, 
        //  arg3, and arg4, respectively. Capture the value returned by fscanf 
        //  in a variable called items_scanned, of type int.
        int items_scanned = fscanf(stream, "%c%d%d%d%d", &command, &arg1, &arg2, &arg3, &arg4);
        //  If the number of items scanned is 1:
        if (items_scanned == 1) 
        {
            //  If the command is 'c', call clear_screen().
            if (command == 'c') {
                clear_screen();
            } else if (command == 's') {
                show_screen();
            }
            //  Otherwise, if the command is 's', call show_screen().
        }

        //  Otherwise, if the number of items scanned is 2:
        else if (items_scanned == 2)
        {
            //  If the command is 'p', assign the value of arg1
            //  into pen.
            if (command == 'p') {
                pen = arg1;
            }
        }

        //  Otherwise, if the number of items scanned is 3:
        else if (items_scanned == 3)
        {
            //  If the command is 'd', draw the pen character at location
            //  (arg1, arg2) on the screen.
            if (command == 'd') {
                draw_char(arg1, arg2, pen);
            }
        }

        //  Otherwise, if the number of items scanned is 5:
        else if (items_scanned == 5)
        {
            //  If the command is 'l', draw a line using the pen character
            //  from (arg1, arg2) to (arg3, arg4).
            if (command == 'l') {
                draw_line(arg1, arg2, arg3, arg4, pen);
            }
        }
    }
}



#define MAX_ITEMS (100)

int main(int argc, char *argv[])
{
    setup_screen();

    for (int i = 1; i < argc; i++) 
    {
        FILE * stream = fopen(argv[i], "r");
        if (stream != NULL)
        {
            show_file(stream);
            fclose(stream);
        }
        draw_string(0, screen_height() - 1, "Press any key to continue...");
        show_screen();
        wait_char();
    }

    return 0;
}
