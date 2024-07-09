#include "server.h"

typedef struct {
  pthread_mutex_t mutex;
} data;

static data* sharedData = NULL;

int main(int argc, char **argv) {

  // read options
  options_t* options = parseOptions(argc, argv);
  if (options == NULL) {
    exit(8);
  }

  // create root directory if missing
  if (!createParentDirectories(options->rootDirectory)) {
    perror("Error while creating root directory");
    exit(7);
  }

  // prepare the connection
  int fd = prepareConnection(options->address, options->port);
  if (fd == -1) {
    exit(1);
  }

  // place the mutex in shared memory
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_SHARED | MAP_ANONYMOUS;
  sharedData = mmap(NULL, sizeof(data), prot, flags, -1, 0);

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&sharedData->mutex, &attr);

  while (true) {
    // accept a connection from the pending requests
    int connectedFd = accept(fd, (struct sockaddr*)NULL, NULL);
    if (connectedFd == -1) {
      perror("Error while accepting a connection from the socket");
    }
    printf("[INFO] Connection has been accepted ...\n");

    pid_t pid = fork();
    if (pid == -1) {
      perror("Error while creating the child process");
    }
    if (pid == 0) {
      // child process
      handleRequest(options, connectedFd);
    } else {
      // parent process
      if (close(connectedFd)) {
        perror("Error while closing the socket");
      }
      printf("[INFO] Connection has been closed ...\n");
    }
  }

  return EXIT_SUCCESS;
}

int handleRequest(options_t* options, int fd) {
  // receive the command from the client
  char* buffer = receiveCommand(fd);
  if (buffer == NULL) {
    exit(4);
  }

  // read the mode and the file path
  char mode = buffer[0];
  char* path = getPath(buffer, options->rootDirectory);
  char* pathWithoutFileName = getRemotePathWithoutFileName(buffer, options->rootDirectory);
  char* fileName = getFileName(buffer);

  // server logic
  if (mode == 'w') {
    // write a file to the server
    writeOperation(fd, path, pathWithoutFileName);
  } else if (mode == 'r') {
    // read a file from the server
    readOperation(fd, path);
  } else if (mode == 'l') {
    // read directories/files from the server
    listOperation(fd, path);
  }

  // close the socket
  if (close(fd)) {
    perror("Error while closing the socket");
    exit(7);
  }

  printf("[INFO] Connection has been closed ...\n");
  return EXIT_SUCCESS;
}

int writeOperation(int fd, char* path, char* pathWithoutFileName) {
  // write a file to the server

  // 1. create the directories of the file path if missing
  if (!createParentDirectories(pathWithoutFileName)) {
    perror("Error while creating the directory");
    return 5;
  }

  // 2. receive the number of bytes of the file (file size)
  int size = receiveFileSize(fd);
  if (size == -1) {
    return 6;
  }

  // 3. receive the binary file

  // TODO: check if the file has been already requested
  // - if yes, apply the mutual exclusion
  // - otherwise go on without mutex

  // FIXME: apply the mutex logic if and only if the file path is the same
  // lock the mutex to avoid race condition
  // and so to implement mutual exclusion
  pthread_mutex_lock(&sharedData->mutex);
  // start critical section
  printf("%d enters in the critical section\n", fd);
  if (!receiveFile(fd, path, size)) {
    return 7;
  }
  printf("%d exits from the critical section\n", fd);
  // unlock the mutex so other threads can enter the critical section
  pthread_mutex_unlock(&sharedData->mutex);
  // end critical section

  return EXIT_SUCCESS;
}

int readOperation(int fd, char* path) {
  // read a file from the server

  // 1. send the number of bytes of the file (file size)
  int size = getFileSize(path);
  if (size == -1) {
    return 5;
  }
  if (!sendFileSize(fd, size)) {
    return 6;
  }

  // 2. send the binary file
  if (!sendFile(fd, path, size)) {
    return 7;
  }

  return EXIT_SUCCESS;
}

int listOperation(int fd, char* path) {
  // read directories/files from the server

  // 1. send the number of files in the directory
  int filesNumber = getDirectoryFilesNumber(path);
  if (filesNumber == -1) {
    return 5;
  }
  if (!sendDirectoryFilesNumber(fd, path)) {
    return 6;
  }

  // 2. send the files in the directory
  if (!sendDirectoryFiles(fd, path)) {
    return 7;
  }

  return EXIT_SUCCESS;
}
