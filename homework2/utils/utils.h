#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // useful to use htonl and htons

#define ADDRESS "127.0.0.1"
#define PORT_NO 5000
#define BUFFER_SIZE 1025

int* createSocket();
struct sockaddr_in* buildAddress(int listenFd);
