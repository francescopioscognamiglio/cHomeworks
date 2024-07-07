#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> // useful to use htonl and htons

#define MAX_PENDING_REQUESTS 100
#define ADDRESS "127.0.0.1"
#define ADDRESS_SIZE 20
#define PORT_NO 5000

int prepareConnection(char* address, int port);
int establishConnection(char* address, int port);
