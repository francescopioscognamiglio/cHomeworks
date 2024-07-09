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
    // write a file to the server
    writeOperation(fd, options->sourcePath, options->targetPath);
  } else if (options->isRead) {
    // read a file from the server
    readOperation(fd, options->sourcePath, options->targetPath);
  } else if (options->isList) {
    // read directories/files from the server
    listOperation(fd, options->sourcePath);
  }

  // close the socket
  if (close(fd)) {
    perror("Error while closing the socket");
    exit(10);
  }

  return EXIT_SUCCESS;
}

int writeOperation(int fd, char* sourcePath, char* targetPath) {
  // write a file to the server

  // 1. send the write command and the file path to be created
  if (!sendCommand(fd, "w", targetPath)) {
    return 4;
  }

  // 2. send the number of bytes of the file (file size)
  int size = getFileSize(sourcePath);
  if (size == -1) {
    return 5;
  }
  if (!sendFileSize(fd, size)) {
    return 6;
  }

  // 3. send the binary file
  if (!sendFile(fd, sourcePath, size)) {
    return 7;
  }

  return EXIT_SUCCESS;
}

int readOperation(int fd, char* sourcePath, char* targetPath) {
  // read a file from the server

  // 1. send the read command and the file path to be read
  if (!sendCommand(fd, "r", sourcePath)) {
    return 4;
  }

  // 2. create the directories of the file path if missing
  char* pathWithoutFileName = getLocalPathWithoutFileName(targetPath);
  if (!createParentDirectories(pathWithoutFileName)) {
    perror("Error while creating the directory");
    return 5;
  }

  // 3. receive the number of bytes of the file (file size)
  int size = receiveFileSize(fd);
  if (size == -1) {
    return 6;
  }

  // 4. receive the binary file
  if (!receiveFile(fd, targetPath, size)) {
    return 7;
  }

  return EXIT_SUCCESS;
}

int listOperation(int fd, char* sourcePath) {
  // read directories/files from the server

  // 1. send the list command
  if (!sendCommand(fd, "l", sourcePath)) {
    return 4;
  }

  // 2. receive the number of files in the directory
  int size = receiveDirectoryFilesNumber(fd);
  if (size == -1) {
    return 6;
  }

  // 3. receive the files in the directory
  if (!receiveDirectoryFiles(fd, sourcePath, size)) {
    return 7;
  }

  return EXIT_SUCCESS;
}
