#include <stdbool.h>

typedef struct {
  bool isWrite;
  bool isRead;
  bool isList;
  char* address;
  int port;
  char* sourcePath;
  char* targetPath;
} options_t;

options_t* parseOptions(int argc, char* argv[]);
