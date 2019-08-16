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

 void show_file ( FILE *stream )
{
    //  Declare a variable called pen of type char, with initial value '*'.
    char pen = '*';

    //  While stream has not reach end of input:
    while(!feof(stream))
    {
        //  Declare a variable called command of type char.
        char command;
        //  Declare six variables of type int.
        int arg1, arg2, arg3, arg4, arg5, arg6;
        //  Use a single call to fscanf to (attempt to) read a char and six int 
        //  values into command, and your integer variables. Capture the value 
        //  returned by fscanf for later use.
        int items_scanned = fscanf(stream, "%c%d%d%d%d%d%d", &command, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6);
        //  If the number of items scanned is 2:
        if (items_scanned == 2)
        {
            //  If the command is 'p', assign the value of arg1
            //  into pen.
            if (command == 'p') {
                pen = arg1;
            }
        }
        //  Otherwise, if the number of items scanned is 5:
        else if (items_scanned == 5)
        {
            //  If the command is 'r', draw a rectangle using the current pen
            //  as indicated in the task statement.
            if (command == 'r') {
                draw_line(arg1, arg2, arg3, arg2, pen);
                draw_line(arg3, arg2, arg3, arg4, pen);

                draw_line(arg3, arg4, arg1, arg4, pen);
                draw_line(arg1, arg4, arg1, arg2, pen);
            }
        }
        //  Otherwise, if the number of items scanned is 7:
        else if (items_scanned == 7)
        {
            //  If the command is 't', draw a closed triangle using the pen 
            //  character with vertices specified by the 6 points, as noted in 
            //  the task statement.
            if (command == 't') {
                draw_line(arg1, arg2, arg3, arg4, pen);
                draw_line(arg3, arg4, arg5, arg6, pen);
                draw_line(arg5, arg6, arg1, arg2, pen);
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
