#include <stdbool.h> // included to use the bool data type

#define USER_LENGTH 100

typedef struct {
  bool isMultiLine;
  bool useRealName;
  bool showSpecialFiles;
} format_t;

typedef struct {
  format_t* format;
  char **users;
  int usersSize;
} finger_t;

int parseOptions(int argc, char* argv[], finger_t* finger);
int parseUsers(int argc, char* argv[], finger_t* finger);
