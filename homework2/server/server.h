#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "../utils/utils.h"
#include "../connection/connection.h"
#include "../sender/sender.h"
#include "../receiver/receiver.h"
#include "parser.h"

int handleRequest(options_t* options, int fd);
int writeOperation(int fd, char* path, char* pathWithoutFileName);
int readOperation(int fd, char* path);
int listOperation(int fd, char* path);
