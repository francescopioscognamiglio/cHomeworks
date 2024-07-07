#include <dirent.h>
#include "../utils/utils.h"

bool sendMessage(int fd, char* buffer, int bufferSize);
bool sendFileSize(int fd, int size);
bool sendCommand(int fd, char* mode, char* path);
bool sendFile(int fd, char* path, int size);
bool sendDirectoryFilesNumber(int fd, char* path);
bool sendDirectoryFiles(int fd, char* path);
