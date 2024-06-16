#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit status
#include <string.h> // included to use string functions
#include <stdbool.h> // included to use the bool data type
#include <unistd.h>
#include <time.h>

int print(finger_t* finger);
int printSingleLine(user_t* user, format_t* format);
int printMultipleLines(user_t* user, format_t* format);
