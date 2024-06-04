#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit status
#include <string.h> // included to use string functions
#include <stdbool.h> // included to use the bool data type
#include "finger.h"

int main(int argc, char* argv[]) {

  int status = EXIT_SUCCESS;

  finger_t* finger = malloc(sizeof(finger_t));
  if (finger == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  status = build(argc, argv, finger);
  if (status == EXIT_FAILURE) {
    printHelp();
    return status;
  }

  // FIXME it's a test, remove it
  printf("Options:\n");
  printf("\tisMultiLine: %d\n", finger->format->isMultiLine);
  printf("\tuseRealName: %d\n", finger->format->useRealName);
  printf("\tshowSpecialFiles: %d\n", finger->format->showSpecialFiles);

  // FIXME it's a test, remove it
  for (int i = 0; i < finger->usersSize; i++) {
    printf("The %dth user is %s\n", i+1, finger->users[i]);
  }

  // release the allocated memory
  release(finger);

  return status;
}

int build(int argc, char* argv[], finger_t* finger) {
  int status = EXIT_SUCCESS;

  // allocate the necessary memory (but first clean it)
  finger->format = malloc(sizeof(format_t));
  if (finger->format == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  char **users = NULL;
  status = parseUsers(argc, argv, finger);
  if (status == EXIT_FAILURE) {
    return status;
  }

  status = parseOptions(argc, argv, finger);
  if (status == EXIT_FAILURE) {
    return status;
  }

  return status;
}

int release(finger_t* finger) {
  // release the allocated memory
  free(finger->format);
  for (int i = 0; i < finger->usersSize; i++) {
    free(finger->users[i]);
  }
  free(finger->users);

  return EXIT_SUCCESS;
}

void printHelp() {
  printf("usage: finger [-lmsp] [user ...]\n");
}
