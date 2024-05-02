#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit status
#include <string.h> // included to use string functions
#include <stdbool.h> // included to use the bool data type
#include "finger.h"

int main(int argc, char* argv[]) {

  // allocate the necessary memory (but first clean it)
  options_t *options = malloc(sizeof(options_t));
  if (options == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  int result = parseArguments(argc, argv, options);
  if (result == EXIT_FAILURE) {
    return result;
  }

  // release the allocated memory
  free(options);

  return result;
}

int parseArguments(int argc, char* argv[], options_t *options) {
  // check the provided arguments
  // skip the first argument that is the program name
  for (int i = 1; i < argc; i++) {
    printf("argc = %d\n", i);
    printf("argv[%d] --> %s\n", i, argv[i]);

    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j] != '\0'; j++) {
        bool found = false;
        if (argv[i][j] == 'l') {
          options->l = true;
          found = true;
        }
        if (argv[i][j] == 'm') {
          options->m = true;
          found = true;
        }
        if (argv[i][j] == 's') {
          options->s = true;
          found = true;
        }
        if (argv[i][j] == 'p') {
          options->p = true;
          found = true;
        }

        if (found == false) {
          printf("finger: invalid option -- '%c'\n", argv[i][j]);
          printHelp();
          return EXIT_FAILURE;
        }
      }
    }
  }

  return EXIT_SUCCESS;
}

void printHelp() {
  printf("usage: finger [-lmps] [login ...]\n");
}
