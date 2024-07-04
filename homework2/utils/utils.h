#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // useful to use htonl and htons

#define MAX_PENDING_REQUESTS 100
#define ADDRESS "127.0.0.1"
#define ADDRESS_SIZE 20
#define PORT_NO 5000
#define BUFFER_SIZE 1025
#define PATH_SIZE 200
#define FILENAME_SIZE 20

int prepareConnection(char* address, int port);
int establishConnection(char* address, int port);

bool sendMessage(int fd, char* buffer, int bufferSize);
bool sendSize(int fd, int size);
bool sendCommand(int fd, char* mode, char* path);
bool sendFile(int fd, char* path, int size);

bool receiveMessage(int fd, char* buffer, int bufferSize);
int receiveSize(int fd);
char* receiveCommand(int fd);
bool receiveFile(int fd, char* path, int size);

bool isExistingFile(char* directory);
bool createParentDirectories(char* directory);
bool createDirectory(char* directory);

char* getPath(char* command, char* rootPath);
char* getRemotePathWithoutFileName(char* command, char* rootPath);
char* getLocalPathWithoutFileName(char* filepath);
char* getFileName(char* command);
int getFileSize(char* path);
