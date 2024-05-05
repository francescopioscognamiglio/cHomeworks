#include <stdbool.h> // included to use the bool data type

#define USER_LENGTH 100

typedef struct {
  bool l;
  bool m;
  bool s;
  bool p;
} options_t;

int parseOptions(int argc, char* argv[], options_t *options);
int parseUsers(int argc, char* argv[], char **users, int *usersSize);
void printHelp();
