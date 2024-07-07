#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "utils.h"

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
