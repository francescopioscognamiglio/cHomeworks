#include <stdio.h>
#include "parser.h"
#include "server.h"

int main(int argc, char **argv) {

  // read options
  options_t* options = parseOptions(argc, argv);
  if (options == NULL) {
    exit(8);
  }

  // create root directory if missing
  if (!createParentDirectories(options->rootDirectory)) {
    perror("Error while creating root directory");
    exit(7);
  }

  // prepare the connection
  int fd = prepareConnection(options->address, options->port);
  if (fd == -1) {
    exit(1);
  }

  while (true) {
    // accept a connection from the pending requests
    int connectedFd = accept(fd, (struct sockaddr*)NULL, NULL);
    if (connectedFd == -1) {
      perror("Error while accepting a connection from the socket");
      exit(4);
    }
    printf("Connection has been accepted ...\n");

    // TODO: read the command from the client
    // TODO: perform the operation
    // TODO: send the result to the client (if needed)

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
