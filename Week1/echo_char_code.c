#include <stdio.h>

void echo_char_code() {
    char ch1;
    printf("Please enter a single character:\n");
    scanf("%c", &ch1);

    printf("The character code of '%c' is %d.\n", ch1, ch1);
}


int main() {
	echo_char_code();
	return 0;
}
