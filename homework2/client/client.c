#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // useful to use htonl and htons
#include <netdb.h> // useful to use gethostbyname
#include "parser.h"
#include "client.h"

int main(int argc, char **argv) {

  // read options
  options_t* options = parseOptions(argc, argv);
  if (options == NULL) {
    return EXIT_FAILURE;
  }

  // establish the connection
  int fd = establishConnection(options->address, options->port);
  if (fd == -1) {
    exit(1);
  }

  // client logic
  if (options->isWrite) {
    // write a file to the server:
    // 1. send the write command and the file path to be created
    if (!sendCommand(fd, "w", options->targetPath)) {
      exit(4);
    }
    // 2. send the number of bytes of the file (file size)
    int size = getFileSize(options->sourcePath);
    if (size == -1) {
      exit(5);
    }
    if (!sendFileSize(fd, size)) {
      exit(6);
    }
    // 3. send the binary file
    if (!sendFile(fd, options->sourcePath, size)) {
      exit(7);
    }
  } else if (options->isRead) {
    // read a file from the server:
    // 1. send the read command and the file path to be read
    if (!sendCommand(fd, "r", options->sourcePath)) {
      exit(4);
    }
    // 2. create the directories of the file path if missing
    char* pathWithoutFileName = getLocalPathWithoutFileName(options->targetPath);
    if (!createParentDirectories(pathWithoutFileName)) {
      perror("Error while creating the directory");
      exit(7);
    }
    // 3. receive the number of bytes of the file (file size)
    int size = receiveFileSize(fd);
    if (size == -1) {
      exit(6);
    }
    // 4. receive the binary file
    if (!receiveFile(fd, options->targetPath, size)) {
      exit(7);
    }
  } else if (options->isList) {
    // read directories/files from the server:
    // 1. send the list command
    if (!sendCommand(fd, "l", options->sourcePath)) {
      exit(4);
    }
    // 2. receive the number of files in the directory
    int size = receiveDirectoryFilesNumber(fd);
    if (size == -1) {
      exit(6);
    }
    // 3. receive the files in the directory
    if (!receiveDirectoryFiles(fd, options->sourcePath, size)) {
      exit(7);
    }
  }

  // close the socket
  if (close(fd)) {
    perror("Error while closing the socket");
    exit(10);
  }

  return EXIT_SUCCESS;
}
