#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit status
#include <string.h> // included to use string functions
#include <stdbool.h> // included to use the bool data type
#include <unistd.h>
#include <sys/stat.h>
#include <utmp.h>
#include <pwd.h>
#include <time.h>

int addInitialUser(char* userName, finger_t* finger);
int addUser(char* userName, finger_t* finger);

int parseOptions(int argc, char* argv[], finger_t* finger);
int parseUsers(int argc, char* argv[], finger_t* finger);
