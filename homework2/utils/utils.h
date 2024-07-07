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

bool sendMessage(int fd, char* buffer, int bufferSize);
bool sendFileSize(int fd, int size);
bool sendCommand(int fd, char* mode, char* path);
bool sendFile(int fd, char* path, int size);
bool sendDirectoryFilesNumber(int fd, char* path);
bool sendDirectoryFiles(int fd, char* path);

bool receiveMessage(int fd, char* buffer, int bufferSize);
int receiveFileSize(int fd);
char* receiveCommand(int fd);
bool receiveFile(int fd, char* path, int size);
int receiveDirectoryFilesNumber(int fd);
bool receiveDirectoryFiles(int fd, char* path, int filesNumber);

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
