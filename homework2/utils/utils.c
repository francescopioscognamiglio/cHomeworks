#include <sys/stat.h>
#include <sys/types.h>
#include "utils.h"

int* createSocket() {
  int fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET is the TCP/IP protocol
                                                  // SOCK_STREAM is the TCP transfer service
  if (fd == -1) {
    perror("Error while opening the socket");
    return NULL;
  }

  printf("Socket has been created ...\n");
  int* fdPtr = &fd;
  return fdPtr;
}

struct sockaddr_in* buildAddress(int fd, char* address, int port) {
  struct sockaddr_in* socketAddress = (struct sockaddr_in*)calloc(1, sizeof(struct sockaddr_in));
  socketAddress->sin_family = AF_INET;
  socketAddress->sin_addr.s_addr = inet_addr(address);
  socketAddress->sin_port = htons(port);
  if (bind(fd, (struct sockaddr*)socketAddress, sizeof(*socketAddress))) {
    perror("Error while setting the address to the socket");
    return NULL;
  }

  printf("Address has been set ...\n");
  return socketAddress;
}

bool isExistingDirectory(char* directory) {
  // check if the directory exists
  if (fopen(directory, "r") == NULL) {
    return false;
  }
  return true;
}

bool createParentDirectories(char* directory) {
  // if the directory exists nothing to do
  if (isExistingDirectory(directory)) {
    return true;
  }

  // create parent directories if missing
  // for example: calling it with "A/B/C" creates the directories:
  // - the directory A
  // - the directory B of the path A/B
  // - the directory C of the path A/B/C
  int i = 0, start = 0;
  do {
    if ((i > 0 && directory[i] == '/') || directory[i] == '\0') {
      // copy the path until the next directory
      char* directoryToCreate = (char*)malloc(i+1);
      strncpy(directoryToCreate, directory, i);
      directoryToCreate[i] = '\0'; // add string termination character
      // check if the directory already exists
      if (!isExistingDirectory(directoryToCreate)) {
        // create the directory
        if (!createDirectory(directoryToCreate)) {
          return false;
        }
      }
      start = i+1;
      i++;
    }
    i++;
  } while (directory[i-1] != '\0');

  return true;
}

bool createDirectory(char* directory) {
  // create parent directory
  if (mkdir(directory, S_IRWXU) == -1) {
    return false;
  }
  return true;
}
