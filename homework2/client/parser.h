#include <stdbool.h>

typedef struct {
  bool write;
  bool read;
  bool list;
  char* address;
  int port;
  char* fPath;
  char* oPath;
} options_t;

options_t* parseOptions(int argc, char* argv[]);
