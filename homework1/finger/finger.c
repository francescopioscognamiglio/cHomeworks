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

  int result = parseOptions(argc, argv, options);
  if (result == EXIT_FAILURE) {
    return result;
  }

  char **users = (char**) calloc(1, sizeof(char*));
  if (users == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }
  int usersSize = 0;
  result = parseUsers(argc, argv, users, &usersSize);
  if (result == EXIT_FAILURE) {
    return result;
  }


  // release the allocated memory
  free(options);
  for (int i = 0; i < usersSize; i++) {
    free(users[i]);
  }
  free(users);

  return result;
}

int parseOptions(int argc, char* argv[], options_t *options) {
  // check the provided arguments
  // skip the first argument that is the program name
  for (int i = 1; i < argc; i++) {

    // check if the argument is an option
    if (argv[i][0] == '-') {
      // a string is an array of characters
      // in which the \0 is the termination character
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

int parseUsers(int argc, char* argv[], char **users, int *usersSize) {
  // check the provided arguments
  // skip the first argument that is the program name
  int count = 0;
  for (int i = 1; i < argc; i++) {

    // check if the argument is a user
    if (argv[i][0] != '-') {

      if (count > 0) {
        // re-allocate memory for new users
        users = (char**) realloc(users, (count + 1) * sizeof(char*));
        if (users == NULL) {
          printf("There was an error allocating the memory.\n");
          return EXIT_FAILURE;
        }
      }
      // allocate memory for user information
      users[count] = (char*) calloc(USER_LENGTH, sizeof(char));
      if (users[count] == NULL) {
        printf("There was an error allocating the memory.\n");
        return EXIT_FAILURE;
      }

      strncpy(users[count], argv[i], USER_LENGTH);
      count++;
    }
  }

  *usersSize = count;
  return EXIT_SUCCESS;
}

void printHelp() {
  printf("usage: finger [-lmps] [login ...]\n");
}
