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
    printf("[INFO] Connection has been accepted ...\n");

    pid_t pid = fork();
    if (pid == -1) {
      perror("Error while creating the child process");
      exit(5);
    }
    if (pid == 0) {
      // child process
      handleRequest(options, connectedFd);
    } else {
      // parent process
      if (close(connectedFd)) {
        perror("Error while closing the socket");
        exit(6);
      }
      printf("[INFO] Connection has been closed ...\n");
    }
  }

  return EXIT_SUCCESS;
}

int handleRequest(options_t* options, int fd) {
  // receive the command from the client
  char* buffer = receiveCommand(fd);
  if (buffer == NULL) {
    exit(4);
  }

  char mode = buffer[0];
  char* pathWithoutFileName = getPathWithoutFileName(buffer, options->rootDirectory);
  char* fileName = getFileName(buffer);

  // TODO: server side:
  if (mode == 'w') {
    // write a file to the server:
    // 1. create the directories of the file path if missing
    if (!createParentDirectories(pathWithoutFileName)) {
      perror("Error while creating the directory");
      exit(7);
    }
    // 2. receive the number of bytes of the file (file size)
    int size = receiveSize(fd);
    if (size == -1) {
      exit(6);
    }
    // 3. receive the binary file
    // 4. (optional) send if the operation is successful
  } else if (mode == 'r') {
    // 2. read a file from the server:
    //    - receive the read command and the file path to read
    //    - send the bytes of the file
  } else if (mode == 'l') {
    // 3. read directories/files from the server:
    //    - receive the list command and the file path to list
    //    - send the list of directories/files into the file path as bytes
  }

  // close the socket
  if (close(fd)) {
    perror("Error while closing the socket");
    exit(7);
  }

  printf("[INFO] Connection has been closed ...\n");
  return EXIT_SUCCESS;
}
