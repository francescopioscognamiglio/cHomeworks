#include <stdlib.h>
#include <stdio.h>
#include "../utils/utils.h"
#include "parser.h"

options_t* parseOptions(int argc, char* argv[]) {
  options_t* options = (options_t*) malloc(sizeof(options_t));
  if (options == NULL) {
    perror("Error allocating the memory");
    return NULL;
  }

  // init options
  options->address = NULL;
  options->port = -1;
  options->rootDirectory = NULL;

  // check the provided arguments
  // skip the first argument that is the program name
  bool setAddress = false, setPort = false, setRootDirectory = false;
  for (int i = 1; i < argc; i++) {

    // check if the argument is an option
    if (argv[i][0] == '-') {
      // a string is an array of characters
      // in which the \0 is the termination character
      if (argv[i][1] == '\0') {
        fprintf(stderr, "Option without name: %s\n", argv[i]);
        return NULL;
      }
      if (argv[i][1] == 'a') {
        setAddress = true;
      } else if (argv[i][1] == 'p') {
        setPort = true;
      } else if (argv[i][1] == 'd') {
        setRootDirectory = true;
      }
    } else if (setAddress) {
      options->address = (char*) calloc(ADDRESS_SIZE, sizeof(char));
      strncpy(options->address, argv[i], ADDRESS_SIZE);
      setAddress = false;
    } else if (setPort) {
      options->port = atoi(argv[i]); // convert string to int
      setPort = false;
    } else if (setRootDirectory) {
      options->rootDirectory = (char*) calloc(PATH_SIZE, sizeof(char));
      strncpy(options->rootDirectory, argv[i], PATH_SIZE);
      setRootDirectory = false;
    } else {
      fprintf(stderr, "Invalid argument: %s\n", argv[i]);
      return NULL;
    }
  }

  // check mandatory options
  if (options->address == NULL) {
      fprintf(stderr, "Missing mandatory option: -a\n");
      return NULL;
  }
  if (options->port == -1) {
      fprintf(stderr, "Missing mandatory option: -p\n");
      return NULL;
  }
  if (options->rootDirectory == NULL) {
      fprintf(stderr, "Missing mandatory option: -d\n");
      return NULL;
  }

  return options;
}
