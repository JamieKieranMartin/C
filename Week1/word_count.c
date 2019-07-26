#include <stdio.h>
#include <ctype.h>

void word_count() {
    int chars = 0, words = 0, lines = 0;
    char curr, prev;

    do {
        curr = getc(stdin);

        if (curr != EOF) {
            chars++;
            if ( curr == '\n') lines++;

            if (chars > 1) {
                if (!isspace(curr)) {
                    if (prev == ' ' || prev == '\n') words++;
                }
            } else if (curr == '\n') words = 0;

            prev = curr;
        }   else if (!isspace(prev)) words++;
    } while (curr != EOF);

    printf("The document contains %d lines, %d words, and %d characters.\n", lines, words, chars);
}

int main() {
	word_count();
	return 0;
}