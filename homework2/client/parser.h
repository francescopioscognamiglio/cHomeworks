#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../utils/utils.h"
#include "../connection/connection.h"

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
