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

int setListenMode(int listenFd);
int handleConnection(int connectedFd);
