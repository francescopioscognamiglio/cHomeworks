#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit states
#include "structure.h"
#include "parser.h"
#include "printer.h"
#include "finger.h"

int main(int argc, char* argv[]) {

  int status = EXIT_SUCCESS;

  // prepare finger structure
  finger_t* finger = (finger_t*) malloc(sizeof(finger_t));
  if (finger == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  // populate finger structure
  status = build(argc, argv, finger);
  if (status == EXIT_FAILURE) {
    printHelp();
    return status;
  }

  // print finger structure
  print(finger);

  // release the allocated memory
  release(finger);

  return status;
}

int build(int argc, char* argv[], finger_t* finger) {
  int status = EXIT_SUCCESS;

  // allocate the necessary memory (but first clean it)
  finger->format = (format_t*) malloc(sizeof(format_t));
  if (finger->format == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  status = parseOptions(argc, argv, finger);
  if (status == EXIT_FAILURE) {
    return status;
  }

  status = parseUsers(argc, argv, finger);
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
  printf("finger: usage: finger [-lmsp] [user ...]\n");
}
