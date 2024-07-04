#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
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

  printf("[INFO] Socket has been created ...\n");
  return fd;
}

int setListenMode(int fd) {
  // set the passive mode (server listens) specifying the maximum pending requests
  if (listen(fd, MAX_PENDING_REQUESTS) == -1) {
    perror("Error while listening the socket");
    return EXIT_FAILURE;
  }

  printf("[INFO] Listen mode has been set ...\n");
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

bool isExistingFile(char* directory) {
  // check if the directory exists
  if (fopen(directory, "r") == NULL) {
    return false;
  }
  return true;
}

bool isDirectory(char* path) {
  struct stat pathStat;
  // retrieve stats on path
  if (stat(path, &pathStat) == -1) {
    perror("Error while opening the path");
    return false;
  }
  // check if it's a directory
  if (!S_ISDIR(pathStat.st_mode)) {
    fprintf(stderr, "The path %s is not a directory\n", path);
    return false;
  }
  return true;
}

bool createParentDirectories(char* directory) {
  // if the directory exists nothing to do
  if (isExistingFile(directory)) {
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
      if (!isExistingFile(directoryToCreate)) {
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
    perror("Error while writing the message to the socket");
    return false;
  }

  printf("[INFO] Message \"%s\" has been sent ...\n", buffer);
  return true;
}

bool sendSize(int fd, int size) {
  // send the size to the socket
  // having the file descriptor, it's possible to use the write system call
  if (write(fd, &size, sizeof(int)) == -1) {
    perror("Error while writing the size to the socket");
    return false;
  }

  printf("[INFO] Size \"%d\" has been sent ...\n", size);
  return true;
}

bool sendCommand(int fd, char* mode, char* path) {
  int bufferSize = PATH_SIZE+2;
  char* buffer = calloc(1, bufferSize);
  strncpy(buffer, mode, 1);
  strncat(buffer, ":", 2);
  strncat(buffer, path, PATH_SIZE);

  return sendMessage(fd, buffer, bufferSize);
}

bool sendFile(int fd, char* path, int size) {
  char* buffer = calloc(1, size);

  // open the file
  int readFd = open(path, O_RDONLY);
  if (readFd == -1) {
    perror("Error while opening the file");
    return false;
  }

  // read the file
  int readBytes = read(readFd, buffer, size);
  if (readBytes == -1) {
    perror("Error while reading the file");
    return false;
  }

  // send the file to the socket
  if (!sendMessage(fd, buffer, size)) {
    return false;
  }

  return true;
}

int getDirectoryFilesNumber(char* path) {
  // check if the path is a directory
  if (!isDirectory(path)) {
    return -1;
  }

  // open the directory
  DIR* dir = opendir(path);
  if (dir == NULL) {
    perror("Error while opening the directory");
    return -1;
  }

  // read number of files in the directory
  int filesNumber = 0;
  struct dirent* file;
  while (file = readdir(dir)) {
    // skip current and parent directories
    if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
      continue;
    }
    filesNumber++;
  }

  return filesNumber;
}

bool sendDirectoryFilesNumber(int fd, char* path) {
  int filesNumber = getDirectoryFilesNumber(path);
  if (filesNumber == -1) {
    return false;
  }

  // send the number of files to the socket
  // having the file descriptor, it's possible to use the write system call
  if (write(fd, &filesNumber, sizeof(int)) == -1) {
    perror("Error while writing the number of files to the socket");
    return false;
  }

  printf("[INFO] Number of files \"%d\" has been sent ...\n", filesNumber);
  return true;
}

bool sendDirectoryFiles(int fd, char* path) {
  // check if the path is a directory
  if (!isDirectory(path)) {
    return false;
  }

  // open the directory
  DIR* dir = opendir(path);
  if (dir == NULL) {
    perror("Error while opening the directory");
  }

  // read and send each file in the directory
  struct dirent* file;
  while (file = readdir(dir)) {
    // skip current and parent directories
    if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
      continue;
    }
    // send the file to the socket
    char* buffer = calloc(1, FILENAME_SIZE);
    strncpy(buffer, file->d_name, FILENAME_SIZE);
    if (!sendMessage(fd, buffer, FILENAME_SIZE)) {
      return false;
    }
  }

  return true;
}

bool receiveMessage(int fd, char* buffer, int bufferSize) {
  // read the message from the socket
  // having the file descriptor, it's possible to use the read system call
  int readBytes = read(fd, buffer, bufferSize);
  if (readBytes == -1) {
    perror("Error while reading the message from the socket");
    return false;
  }

  printf("[INFO] Message \"%s\" has been received ...\n", buffer);
  return true;
}

int receiveSize(int fd) {
  // read the size from the socket
  // having the file descriptor, it's possible to use the read system call
  int size = -1;
  int readBytes = read(fd, &size, sizeof(int));
  if (readBytes == -1) {
    perror("Error while reading the size from the socket");
    return -1;
  }

  if (size == -1) {
    fprintf(stderr, "The size of the file is unknown\n");
  }

  printf("[INFO] Size \"%d\" has been received ...\n", size);
  return size;
}

char* receiveCommand(int fd) {
  int bufferSize = PATH_SIZE+2;
  char* buffer = calloc(1, bufferSize);
  if (!receiveMessage(fd, buffer, bufferSize)) {
    return NULL;
  }

  return buffer;
}

bool receiveFile(int fd, char* path, int size) {
  char* buffer = calloc(1, size);

  // receive the file from the socket
  if (!receiveMessage(fd, buffer, size)) {
    return false;
  }

  // check if the file exists
  if (access(path, F_OK) == 0) {
    fprintf(stderr, "The file %s already exists\n", path);
    return false;
  }

  // open the file
  // create it with O_CREAT flag
  // set the read, write and execute permissions to the owner
  int writeFd = open(path, O_WRONLY | O_CREAT, S_IRWXU);
  if (writeFd == -1) {
    perror("Error while opening the file");
    return false;
  }

  // write the file
  if (write(writeFd, buffer, size) == -1) {
    perror("Error while writing the file");
    return false;
  }

  return true;
}

int receiveDirectoryFilesNumber(int fd) {
  // read the number of files from the socket
  // having the file descriptor, it's possible to use the read system call
  int filesNumber = -1;
  int readBytes = read(fd, &filesNumber, sizeof(int));
  if (readBytes == -1) {
    perror("Error while reading the number of files from the socket");
    return -1;
  }

  if (filesNumber == -1) {
    fprintf(stderr, "The number of files is unknown\n");
  }

  printf("[INFO] Number of files \"%d\" has been received ...\n", filesNumber);
  return filesNumber;
}

bool receiveDirectoryFiles(int fd, char* path, int filesNumber) {
  if (filesNumber == -1) {
    printf("The directory %s is empty\n", path);
    return true;
  }

  // iterate the number of files
  printf("The content of the directory %s is:\n", path);
  for (int i = 0; i < filesNumber; i++) {
    // receive the file name from the socket
    char* buffer = calloc(1, FILENAME_SIZE);
    if (!receiveMessage(fd, buffer, FILENAME_SIZE)) {
      return false;
    }
    printf("\t%s\n", buffer);
  }

  return true;
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

char* getPath(char* command, char* rootPath) {
  char* path = calloc(1, PATH_SIZE*2);

  // the first character is the mode, so skip it
  // the second character is the separator ':', so skip it
  // the path starts from the third character
  char* pathToCopy = &command[2];

  // build the absolute path:
  // append the root path
  strncpy(path, rootPath, PATH_SIZE);
  // append a file separator
  strncat(path, "/", 2);
  // append the extracted path
  strncat(path, pathToCopy, PATH_SIZE);

  return path;
}

char* getRemotePathWithoutFileName(char* command, char* rootPath) {
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

char* getLocalPathWithoutFileName(char* filepath) {
  char* path = calloc(1, PATH_SIZE);

  // retrieve the last file separator '/' in order to skip the file name
  int pathEnd = getIndexOfLastFileSeparator(filepath)-1;

  // build the absolute path without the file name
  strncpy(path, filepath, pathEnd);

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

int getFileSize(char* path) {
  struct stat st;
  if (stat(path, &st) == 0) {
    return st.st_size;
  }

  fprintf(stderr, "Error while reading the size of the file %s\n", path);
  perror("Error");
  return -1;
}
