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

  // TODO: client side:
  if (options->isWrite) {
    // 1. write a file to the server:
    //    - send the write command and the file path to be created
    if (!sendCommand(fd, "w", options->targetPath)) {
      exit(4);
    }
    //    - send the bytes of the file
    //    - (optional) receive if the operation is successful
  } else if (options->isRead) {
    // 2. read a file from the server:
    //    - send the read command and the file path to read
    if (!sendCommand(fd, "r", options->sourcePath)) {
      exit(4);
    }
    //    - receive the bytes of the file
    //    - write the file to the file path
  } else if (options->isList) {
    // 3. read directories/files from the server:
    //    - send the list command
    if (!sendCommand(fd, "l", options->sourcePath)) {
      exit(4);
    }
    //    - receive the list of directories/files into the file path as bytes
  }

  // close the socket
  if (close(fd)) {
    perror("Error while closing the socket");
    exit(10);
  }

  return EXIT_SUCCESS;
}
