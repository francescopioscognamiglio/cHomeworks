#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // useful to use htonl and htons
#include "../utils/utils.h"

#define MAX_PENDING_REQUESTS 100

int setListenMode(int listenFd);
int handleConnection(int connectedFd);
