#include "../utils/utils.h"
#include <sys/file.h>

bool receiveMessage(int fd, char* buffer, int bufferSize);
int receiveFileSize(int fd);
char* receiveCommand(int fd);
bool receiveFile(int fd, char* path, int size);
int receiveDirectoryFilesNumber(int fd);
bool receiveDirectoryFiles(int fd, char* path, int filesNumber);
