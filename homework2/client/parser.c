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
  options->write = false;
  options->read = false;
  options->list = false;
  options->address = NULL;
  options->port = -1;
  options->sourcePath = NULL;
  options->targetPath = NULL;

  // check the provided arguments
  // skip the first argument that is the program name
  for (int i = 1; i < argc; i++) {

    // check if the argument is an option
    if (argv[i][0] == '-') {
      // check the option name
      if (argv[i][1] == '\0') {
        fprintf(stderr, "Option without name: %s\n", argv[i]);
        return NULL;
      }
      // check the option value
      if ((i+1) >= argc || argv[i+1][0] == '\0') {
        fprintf(stderr, "Option without value: %s\n", argv[i]);
        return NULL;
      }

      // set the value
      if (argv[i][1] == 'w') {
        options->write = true;
      } else if (argv[i][1] == 'r') {
        options->read = true;
      } else if (argv[i][1] == 'l') {
        options->list = true;
      } else if (argv[i][1] == 'a') {
        options->address = (char*) calloc(ADDRESS_SIZE, sizeof(char));
        strncpy(options->address, argv[++i], ADDRESS_SIZE);
      } else if (argv[i][1] == 'p') {
        options->port = atoi(argv[++i]); // convert string to int
      } else if (argv[i][1] == 'f') {
        options->sourcePath = (char*) calloc(PATH_SIZE, sizeof(char));
        strncpy(options->sourcePath, argv[++i], PATH_SIZE);
      } else if (argv[i][1] == 'o') {
        options->targetPath = (char*) calloc(PATH_SIZE, sizeof(char));
        strncpy(options->targetPath, argv[++i], PATH_SIZE);
      } else {
        fprintf(stderr, "Invalid option: %s\n", argv[i]);
        return NULL;
      }
    } else {
      fprintf(stderr, "Invalid argument: %s\n", argv[i]);
      return NULL;
    }
  }

  // check exclusive options
  if (options->write && options->read && options->list) {
    fprintf(stderr, "Specify only one between: -w -r -l\n");
  } if (options->write && options->read) {
    fprintf(stderr, "Specify only one between: -w -r\n");
  } else if (options->write && options->list) {
    fprintf(stderr, "Specify only one between: -w -l\n");
  } else if (options->read && options->list) {
    fprintf(stderr, "Specify only one between: -r -l\n");
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
  if (options->sourcePath == NULL) {
      fprintf(stderr, "Missing mandatory option: -f\n");
      return NULL;
  }

  // if the target path is missing, then set it to the source path
  if (options->targetPath == NULL) {
    options->targetPath = (char*) calloc(PATH_SIZE, sizeof(char));
    strncpy(options->targetPath, options->sourcePath, PATH_SIZE);
  }

  return options;
}
