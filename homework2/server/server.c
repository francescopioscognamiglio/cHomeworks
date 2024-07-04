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

  // read the mode and the file path
  char mode = buffer[0];
  char* path = getPath(buffer, options->rootDirectory);
  char* pathWithoutFileName = getRemotePathWithoutFileName(buffer, options->rootDirectory);
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
    int size = receiveFileSize(fd);
    if (size == -1) {
      exit(6);
    }
    // 3. receive the binary file
    if (!receiveFile(fd, path, size)) {
      exit(7);
    }
  } else if (mode == 'r') {
    // read a file from the server:
    // 1. send the number of bytes of the file (file size)
    int size = getFileSize(path);
    if (size == -1) {
      exit(5);
    }
    if (!sendFileSize(fd, size)) {
      exit(6);
    }
    // 2. send the binary file
    if (!sendFile(fd, path, size)) {
      exit(7);
    }
  } else if (mode == 'l') {
    // read directories/files from the server:
    // 1. send the number of files in the directory
    int filesNumber = getDirectoryFilesNumber(path);
    if (filesNumber == -1) {
      exit(5);
    }
    if (!sendDirectoryFilesNumber(fd, path)) {
      exit(6);
    }
    // 2. send the files in the directory
    if (!sendDirectoryFiles(fd, path)) {
      exit(7);
    }
  }

  // close the socket
  if (close(fd)) {
    perror("Error while closing the socket");
    exit(7);
  }

  printf("[INFO] Connection has been closed ...\n");
  return EXIT_SUCCESS;
}
