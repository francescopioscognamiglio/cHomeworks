#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit status
#include <string.h> // included to use string functions
#include <stdbool.h> // included to use the bool data type
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include "parser.h"

int build(int argc, char* argv[], finger_t* finger);
int release(finger_t* finger);
void printHelp();
