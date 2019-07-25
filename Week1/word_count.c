#include <stdio.h>
#include <ctype.h>

void word_count() {
    //  Declare an integer variable which will be used to count characters.
    //  Initialise the variable to zero.

    //  Enter a loop which continues indefinitely.
            // Read a single character code from standard input.

        // If the character code is equal to EOF, break out of the loop.

        // Increment the counter by 1.
    // End the body of the loop.

    // Display the number of characters processed.
    int char_count = 0;
    int count = 1;
    int line_count = 0;
    char prev_char;
    while (1 == 1) {
        char ch1 = getchar();
    
        if (ch1 == EOF) {
            break;
        }

        if (ch1 == '\n') {
            line_count++;
        } else if (isspace(prev_char) && !isspace(ch1)) {
            count++;
        }

        prev_char = ch1;
        char_count++;
    }
    printf("The document contains %d lines, %d words, and %d characters.\n", line_count, count, char_count);
}


int main() {
	word_count();
	return 0;
}
