#include <sys/stat.h>
#include <sys/types.h>
#include "utils.h"

int createSocket() {
  // create the socket with the following parameters:
  // - AF_INET is the TCP/IP protocol
  // - SOCK_STREAM is the TCP transfer service
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("Error while opening the socket");
    return -1;
  }

  printf("Socket has been created ...\n");
  return fd;
}

int setListenMode(int fd) {
  // set the passive mode (server listens) specifying the maximum pending requests
  if (listen(fd, MAX_PENDING_REQUESTS) == -1) {
    perror("Error while listening the socket");
    return EXIT_FAILURE;
  }

  printf("Listen mode has been set ...\n");
  return EXIT_SUCCESS;
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
  if (setListenMode(fd) == EXIT_FAILURE) {
    return -1;
  }

  printf("The connection has been prepared ...\n");
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

  printf("The connection has been established ...\n");
  return fd;
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

bool sendMessage(int fd, char* buffer, int bufferSize) {
  // send the message to the socket
  // having the file descriptor, it's possible to use the write system call
  if (write(fd, buffer, bufferSize) == -1) {
    perror("Error while writing to the socket");
    return false;
  }

  printf("Message \"%s\" has been sent ...\n", buffer);
  return true;
}

bool sendCommand(int fd, char* mode, char* path) {
  int bufferSize = PATH_SIZE+2;
  char* buffer = calloc(1, bufferSize);
  strncpy(buffer, mode, 1);
  strncat(buffer, ":", 2);
  strncat(buffer, path, PATH_SIZE);

  printf("Sending command: %s\n", buffer);
  return sendMessage(fd, buffer, bufferSize);
}

bool receiveMessage(int fd, char* buffer, int bufferSize) {
  // read the message from the socket
  // having the file descriptor, it's possible to use the read system call
  int readBytes = read(fd, buffer, bufferSize);
  if (readBytes == -1) {
    perror("Error while reading from the socket");
    return false;
  }

  printf("Message \"%s\" has been received ...\n", buffer);
  return true;
}

char* receiveCommand(int fd) {
  int bufferSize = PATH_SIZE+2;
  char* buffer = calloc(1, bufferSize);
  if (!receiveMessage(fd, buffer, bufferSize)) {
    return NULL;
  }

  printf("Reading command: %s\n", buffer);
  return buffer;
}

int getIndexOfLastFileSeparator(char* command) {
  // retrieve the last file separator '/' in order to skip the file name
  int lastFileSeparator = -1;
  for (int i = 0; command[i] != '\0'; i++) {
    if (command[i] == '/') {
      lastFileSeparator = i;
    }
  }

  return lastFileSeparator;
}

char* getPathWithoutFileName(char* command, char* rootPath) {
  char* path = calloc(1, PATH_SIZE*2);

  // the first character is the mode, so skip it
  // the second character is the separator ':', so skip it
  // the path starts from the third character
  char* pathToCopy = &command[2];

  // retrieve the last file separator '/' in order to skip the file name
  int pathEnd = getIndexOfLastFileSeparator(command)-1;

  // build the absolute path:
  // append the root path
  strncpy(path, rootPath, PATH_SIZE);
  // append a file separator
  strncat(path, "/", 2);
  // append the extracted path
  strncat(path, pathToCopy, pathEnd);

  return path;
}

char* getFileName(char* command) {
  char* fileName = calloc(1, FILENAME_SIZE);

  // retrieve the last file separator '/' in order to skip the directories path
  int fileNameStart = getIndexOfLastFileSeparator(command)+1;
  char* fileNameToCopy = &command[fileNameStart];

  // append the file name
  strncpy(fileName, fileNameToCopy, FILENAME_SIZE);

  return fileName;
}
