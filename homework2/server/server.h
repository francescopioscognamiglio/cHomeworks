#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/mman.h>
#include "../utils/utils.h"
#include "../connection/connection.h"
#include "parser.h"

int handleRequest(options_t* options, int fd);
