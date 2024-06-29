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
#define ADDRESS_SIZE 20
#define PORT_NO 5000
#define BUFFER_SIZE 1025
#define PATH_SIZE 200

int* createSocket();
struct sockaddr_in* buildAddress(int listenFd, char* address, int port);
