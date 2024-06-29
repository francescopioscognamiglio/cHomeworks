#include "server.h"

int main(int argc, char **argv) {
  // create the socket
  int* listenFdPtr = createSocket();
  if (listenFdPtr == NULL) {
    exit(1);
  }
  int listenFd = *listenFdPtr;

  // build the address
  struct sockaddr_in* serverAddress = buildAddress(listenFd);
  if (serverAddress == NULL) {
    exit(2);
  }

  // set the listen mode
  if (setListenMode(listenFd) == EXIT_FAILURE) {
    exit(3);
  }

  while (true) {
    // accept a connection from the pending requests
    int connectedFd = accept(listenFd, (struct sockaddr*)NULL, NULL);
    if (connectedFd == -1) {
      perror("Error while accepting a connection from the socket");
      exit(4);
    }
    printf("Connection has been accepted ...\n");

    pid_t pid = fork();
    if (pid == -1) {
      perror("Error while creating the child process");
      exit(5);
    }
    if (pid == 0) {
      // child process
      handleConnection(connectedFd);
    } else {
      // parent process
      if (close(connectedFd)) {
        perror("Error while closing the socket");
        exit(6);
      }
      printf("Connection has been closed ...\n");
    }
  }

  return EXIT_SUCCESS;
}

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

struct sockaddr_in* buildAddress(int listenFd) {
  struct sockaddr_in* serverAddress;
  memset(serverAddress, 0, sizeof(serverAddress)); // be sure that the area is cleaned
  serverAddress->sin_family = AF_INET;
  serverAddress->sin_addr.s_addr = inet_addr(ADDRESS);
  serverAddress->sin_port = htons(PORT_NO);
  if (bind(listenFd, (struct sockaddr*)serverAddress, sizeof(serverAddress))) {
    perror("Error while setting the address to the socket");
    return NULL;
  }

  printf("Address has been set ...\n");
  return serverAddress;
}

int setListenMode(int listenFd) {
  // set passive mode (server listens) and the maximum pending requests
  if (listen(listenFd, MAX_PENDING_REQUESTS) == -1) {
    perror("Error while listening the socket");
    return EXIT_FAILURE;
  }

  printf("Listen mode has been set ...\n");
  return EXIT_SUCCESS;
}

int handleConnection(int connectedFd) {
  char sendBuffer[BUFFER_SIZE];
  memset(sendBuffer, 0, sizeof(sendBuffer)); // be sure that the area is cleaned

  // useful to perform calculations without blocking other pending requests
  snprintf(sendBuffer, sizeof(sendBuffer), "This is the message to the client");

  // send the message to the socket
  // since I have the file descriptor, I can use the write system call instead of the send one
  if (write(connectedFd, sendBuffer, strlen(sendBuffer)) == -1) {
    perror("Error while writing to the socket");
    exit(6);
  }
  printf("Message \"%s\" has been sent ...\n", sendBuffer);

  // close the socket
  if (close(connectedFd)) {
    perror("Error while closing the socket");
    exit(7);
  }

  printf("Connection has been closed ...\n");
  return EXIT_SUCCESS;
}
