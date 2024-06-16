#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit status
#include <string.h> // included to use string functions
#include <stdbool.h> // included to use the bool data type
#include <unistd.h>
#include <sys/stat.h>
#include <utmp.h>
#include <pwd.h>
#include <time.h>

typedef struct {
  bool isMultiLine;
  bool useRealName;
  bool showSpecialFiles;
} format_t;

typedef struct {
  char* loginName;
  char* realName;
  char* terminalName;
  char* terminalSuffix;
  //char* writeStatus;
  int idleTimeHours;
  int idleTimeMinutes;
  int loginDate;
  char* officeLocation;
  char* officePhoneNumber;
  char* homeDirectory;
  char* homePhoneNumber;
  char* loginShell;
  char* mail;
  char* plan;
  char* project;
  char* pgpkey;
  char* forward;
} user_t;

typedef struct {
  format_t* format;
  user_t** users;
  int usersSize;
} finger_t;

int addInitialUser(char* userName, finger_t* finger);
int addUser(char* userName, finger_t* finger);

int parseOptions(int argc, char* argv[], finger_t* finger);
int parseUsers(int argc, char* argv[], finger_t* finger);
