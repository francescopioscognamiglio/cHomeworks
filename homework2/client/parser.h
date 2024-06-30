#include <stdbool.h>

typedef struct {
  bool write;
  bool read;
  bool list;
  char* address;
  int port;
  char* sourcePath;
  char* targetPath;
} options_t;

options_t* parseOptions(int argc, char* argv[]);
