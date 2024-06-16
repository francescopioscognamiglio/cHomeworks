#include <stdbool.h> // included to use the bool data type

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
