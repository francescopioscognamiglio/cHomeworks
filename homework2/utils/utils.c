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
