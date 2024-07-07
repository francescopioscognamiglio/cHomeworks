#include "connection.h"

int createSocket() {
  // create the socket with the following parameters:
  // - AF_INET is the TCP/IP protocol
  // - SOCK_STREAM is the TCP transfer service
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("Error while opening the socket");
    return -1;
  }

  printf("[INFO] Socket has been created ...\n");
  return fd;
}

int setListenMode(int fd) {
  // set the passive mode (server listens) specifying the maximum pending requests
  if (listen(fd, MAX_PENDING_REQUESTS) == -1) {
    perror("Error while listening the socket");
    return -1;
  }

  printf("[INFO] Listen mode has been set ...\n");
  return 0;
}

int prepareConnection(char* address, int port) {
  // prepare the connection:
  // - create the socket
  // - assign the address to the socket
  // - set the listening mode to accept connections from the clients

  // create the socket
  int fd = createSocket();
  if (fd == -1) {
    return -1;
  }

  // assign the address
  struct sockaddr_in* socketAddress = (struct sockaddr_in*)calloc(1, sizeof(struct sockaddr_in));
  socketAddress->sin_family = AF_INET;
  socketAddress->sin_addr.s_addr = inet_addr(address);
  socketAddress->sin_port = htons(port);
  // bind operation:
  // assign an address to the socket
  // this operation is usually performed by the server to prepare the socket to accept connections
  if (bind(fd, (struct sockaddr*)socketAddress, sizeof(*socketAddress))) {
    perror("Error while setting the address to the socket");
    return -1;
  }

  // set the listening mode
  if (setListenMode(fd) == -1) {
    return -1;
  }

  printf("[INFO] The connection has been prepared ...\n");
  return fd;
}

int establishConnection(char* address, int port) {
  // establish the connection:
  // - create the socket
  // - assign the address to the socket
  // - connect to the already opened socket

  // create the socket
  int fd = createSocket();
  if (fd == -1) {
    return -1;
  }

  // assign the address
  struct sockaddr_in* socketAddress = (struct sockaddr_in*)calloc(1, sizeof(struct sockaddr_in));
  socketAddress->sin_family = AF_INET;
  socketAddress->sin_addr.s_addr = inet_addr(address);
  socketAddress->sin_port = htons(port);
  // connect operation:
  // connect to the opened socket
  // this operation is usually performed by the client to connect to a socket opened by the server
  if (connect(fd, (struct sockaddr*)socketAddress, sizeof(*socketAddress))) {
    perror("Error while setting the address to the socket");
    return -1;
  }

  printf("[INFO] The connection has been established ...\n");
  return fd;
}
