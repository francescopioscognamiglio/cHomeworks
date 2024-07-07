#include <stdlib.h>
#include <stdio.h>
#include "../utils/utils.h"
#include "../connection/connection.h"

typedef struct {
  char* address;
  int port;
  char* rootDirectory;
} options_t;

options_t* parseOptions(int argc, char* argv[]);
