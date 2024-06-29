#include "utils.h"

int* createSocket() {
  int listenFd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET is the TCP/IP protocol
                                                  // SOCK_STREAM is the TCP transfer service
  if (listenFd == -1) {
    perror("Error while opening the socket");
    return NULL;
  }

  printf("Socket has been created ...\n");
  int* listenFdPtr = &listenFd;
  return listenFdPtr;
}

struct sockaddr_in* buildAddress(int listenFd, char* address, int port) {
  struct sockaddr_in* serverAddress;
  memset(serverAddress, 0, sizeof(serverAddress)); // be sure that the area is cleaned
  serverAddress->sin_family = AF_INET;
  serverAddress->sin_addr.s_addr = inet_addr(address);
  serverAddress->sin_port = htons(port);
  if (bind(listenFd, (struct sockaddr*)serverAddress, sizeof(serverAddress))) {
    perror("Error while setting the address to the socket");
    return NULL;
  }

  printf("Address has been set ...\n");
  return serverAddress;
}
