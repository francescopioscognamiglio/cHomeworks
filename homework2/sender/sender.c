#include "sender.h"

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

bool sendFileSize(int fd, int size) {
  // send the file size to the socket
  // having the file descriptor, it's possible to use the write system call
  if (write(fd, &size, sizeof(int)) == -1) {
    perror("Error while writing the file size to the socket");
    return false;
  }

  printf("[INFO] File size \"%d\" has been sent ...\n", size);
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
