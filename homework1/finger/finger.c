#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit states and memory allocation functions
#include "structure.h"
#include "parser.h"
#include "printer.h"
#include "finger.h"

int main(int argc, char* argv[]) {
  // prepare finger structure
  finger_t* finger = (finger_t*) malloc(sizeof(finger_t));
  if (finger == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  // populate finger structure
  if (build(argc, argv, finger) == EXIT_FAILURE) {
    printHelp();
    return EXIT_FAILURE;
  }

  // print finger structure
  if (print(finger) == EXIT_FAILURE) {
    printHelp();
    return EXIT_FAILURE;
  }

  // release the allocated memory
  release(finger);

  return EXIT_SUCCESS;
}

int build(int argc, char* argv[], finger_t* finger) {
  // allocate the necessary memory (but first clean it)
  finger->format = (format_t*) malloc(sizeof(format_t));
  if (finger->format == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  if (parseOptions(argc, argv, finger) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (parseUsers(argc, argv, finger) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
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
  printf("finger: usage: finger [-lmsp] [user ...]\n");
}
