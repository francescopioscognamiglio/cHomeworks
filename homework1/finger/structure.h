#include <stdbool.h> // included to use the bool data type

typedef struct {
  bool isMultiLine;
  bool useRealName;
  bool showSpecialFiles;
  bool foundStyle;
} format_t;

typedef struct {
  int hours;
  int minutes;
  int seconds;
} idletime_t;

typedef struct {
  char* loginName;
  char* realName;
  char* terminalName;
  char* terminalSuffix;
  //char* writeStatus;
  idletime_t* idleTime;
  int loginDate;
  char* officeLocation;
  char* officePhone;
  char* homePhone;
  char* homeDirectory;
  char* homePhoneNumber;
  char* loginShell;
} user_t;

typedef struct {
  format_t* format;
  user_t** users;
  int usersSize;
  char** uniqueUsers;
  int uniqueUsersSize;
} finger_t;
