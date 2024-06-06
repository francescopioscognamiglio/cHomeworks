#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit status
#include <string.h> // included to use string functions
#include <stdbool.h> // included to use the bool data type
#include "parser.h"

int parseOptions(int argc, char* argv[], finger_t* finger) {
  // init format information
  finger->format->isMultiLine = false;
  finger->format->useRealName = true;
  finger->format->showSpecialFiles = true;

  // check the provided arguments
  // skip the first argument that is the program name
  bool foundStyle = false;
  for (int i = 1; i < argc; i++) {

    // check if the argument is an option
    if (argv[i][0] == '-') {
      // a string is an array of characters
      // in which the \0 is the termination character
      for (int j = 1; argv[i][j] != '\0'; j++) {
        bool found = false;
        if (argv[i][j] == 'l') {
          finger->format->isMultiLine = true;
          found = true;
          foundStyle = true;
        }
        if (argv[i][j] == 'm') {
          finger->format->useRealName = false;
          found = true;
        }
        if (argv[i][j] == 's') {
          if (foundStyle == false) {
            finger->format->isMultiLine = false;
          }
          found = true;
          foundStyle = true;
        }
        if (argv[i][j] == 'p') {
          finger->format->showSpecialFiles = false;
          found = true;
        }

        if (found == false) {
          printf("finger: invalid option -- '%c'\n", argv[i][j]);
          return EXIT_FAILURE;
        }
      }
    }
  }

  if (foundStyle == false) {
    if (finger->usersSize > 0) {
      finger->format->isMultiLine = true;
    } else {
      finger->format->isMultiLine = false;
    }
  }

  return EXIT_SUCCESS;
}

int parseUsers(int argc, char* argv[], finger_t* finger) {
  int status = EXIT_SUCCESS;
  finger->usersSize = 0;

  // check the provided arguments
  // skip the first argument that is the program name
  for (int i = 1; i < argc; i++) {
    // check if the argument is a user
    if (argv[i][0] != '-') {
      status = addUser(argv[i], finger);
      if (status == EXIT_FAILURE) {
        return status;
      }
    }
  }

  return EXIT_SUCCESS;
}

int addInitialUser(char* user, finger_t* finger) {
  finger->usersSize = 0;
  return addUser(user, finger);
}

int addUser(char* user, finger_t* finger) {
  if (finger->usersSize == 0) {
    // allocate memory for the first user
    finger->users = (char**) calloc(1, sizeof(char*));
  } else if (finger->usersSize > 0) {
    // re-allocate memory for new users
    finger->users = (char**) realloc(finger->users, (finger->usersSize + 1) * sizeof(char*));
  }

  if (finger->users == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  // allocate memory for user information
  finger->users[finger->usersSize] = (char*) calloc(UT_NAMESIZE, sizeof(char));
  if (finger->users[finger->usersSize] == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  strncpy(finger->users[finger->usersSize], user, UT_NAMESIZE);
  finger->usersSize++;
  return EXIT_SUCCESS;
}