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

  // create the socket
  int* clientFdPtr = createSocket();
  if (clientFdPtr == NULL) {
    exit(1);
  }
  int clientFd = *clientFdPtr;

  // build the address
  struct sockaddr_in* serverAddress = connectOperation(clientFd, options->address, options->port);
  if (serverAddress == NULL) {
    exit(2);
  }

  char readBuffer[BUFFER_SIZE];
  memset(readBuffer, 0, sizeof(readBuffer)); // be sure that the area is cleaned

  // read the message from the socket
  // since I have the file descriptor, I can use the read system call instead of the recv one
  int readBytes = read(clientFd, readBuffer, BUFFER_SIZE);
  if (readBytes == -1) {
    perror("Error while reading from the socket");
    exit(4);
  }
  printf("Read %d bytes from the server ...\n", readBytes);
  printf("Message received from the server: %s\n", readBuffer);

  // close the socket
  if (close(clientFd)) {
    perror("Error while closing the socket");
    exit(5);
  }

  return EXIT_SUCCESS;
}
