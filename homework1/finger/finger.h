#include <stdbool.h> // included to use the bool data type

typedef struct {
  bool l;
  bool m;
  bool s;
  bool p;
} options_t;

int parseArguments(int argc, char* argv[], options_t *options);
void printHelp();
