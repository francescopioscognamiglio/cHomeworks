#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h> // useful to use gethostbyname
#include "../utils/utils.h"
#include "../connection/connection.h"
#include "../sender/sender.h"
#include "../receiver/receiver.h"
#include "parser.h"

int writeOperation(int fd, char* sourcePath, char* targetPath);
int readOperation(int fd, char* sourcePath, char* targetPath);
int listOperation(int fd, char* sourcePath);
