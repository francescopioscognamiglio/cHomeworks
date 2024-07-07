#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_SIZE 1025
#define PATH_SIZE 200
#define FILENAME_SIZE 20

bool isExistingFile(char* directory);
bool isDirectory(char* path);
bool createParentDirectories(char* directory);
bool createDirectory(char* directory);

char* getPath(char* command, char* rootPath);
char* getRemotePathWithoutFileName(char* command, char* rootPath);
char* getLocalPathWithoutFileName(char* filepath);
char* getFileName(char* command);
int getFileSize(char* path);
int getDirectoryFilesNumber(char* path);
