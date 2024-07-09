#include "receiver.h"

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

int receiveFileSize(int fd) {
  // read the file size from the socket
  // having the file descriptor, it's possible to use the read system call
  int size = -1;
  int readBytes = read(fd, &size, sizeof(int));
  if (readBytes == -1) {
    perror("Error while reading the file size from the socket");
    return -1;
  }

  if (size == -1) {
    fprintf(stderr, "The file size is unknown\n");
  }

  printf("[INFO] File size \"%d\" has been received ...\n", size);
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

  // open the file
  // create it with O_CREAT flag
  // if already exists, write at the end of the file
  // set the read, write and execute permissions to the owner
  int writeFd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
  if (writeFd == -1) {
    perror("Error while opening the file");
    return false;
  }


  // implement mutual exclusion on file

  // place an exclusive lock
  int status = flock(writeFd, LOCK_EX);

  // start critical section

  printf("[INFO] entering in the critical section for \"%s\" ...\n", path);

  // read the entire file from the socket
  int totalReadBytes = 0;
  do {
    // allocate the space for the buffer
    char* buffer = calloc(1, size);

    // having the file descriptor, it's possible to use the read system call
    int readBytes = read(fd, buffer, size);
    if (readBytes == -1) {
      perror("Error while reading the file from the socket");
      return false;
    }

    printf("[INFO] Read \"%d\" bytes of the file ...\n", readBytes);

    // write the file
    if (write(writeFd, buffer, readBytes) == -1) {
      perror("Error while writing the file");
      return false;
    }

    // clean the space allocated for the buffer
    free(buffer);

    totalReadBytes += readBytes;
  } while ((size - totalReadBytes) > 0);

  printf("[INFO] exit from the critical section for \"%s\" .\n", path);

  // remove the exclusive lock
  flock(writeFd, LOCK_UN);

  // end critical section

  // close the file
  if (close(writeFd) == -1) {
    perror("Error while closing the file");
    return false;
  }

  printf("[INFO] File with size \"%d\" has been received ...\n", size);
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
