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
  for (int i = 1; i < argc; i++) {

    // check if the argument is an option
    if (argv[i][0] == '-') {
      // check the option name
      if (argv[i][1] == '\0') {
        fprintf(stderr, "Option without name: %s\n", argv[i]);
        return NULL;
      }
      // check the option value
      if ((i+1) >= argc || argv[i+1][0] == '\0' || argv[i+1][0] == '-') {
        fprintf(stderr, "Option without value: %s\n", argv[i]);
        return NULL;
      }

      // set the value
      if (argv[i][1] == 'a') {
        options->address = (char*) calloc(ADDRESS_SIZE, sizeof(char));
        strncpy(options->address, argv[++i], ADDRESS_SIZE);
      } else if (argv[i][1] == 'p') {
        options->port = atoi(argv[++i]); // convert string to int
      } else if (argv[i][1] == 'd') {
        options->rootDirectory = (char*) calloc(PATH_SIZE, sizeof(char));
        strncpy(options->rootDirectory, argv[++i], PATH_SIZE);
      } else {
        fprintf(stderr, "Invalid option: %s\n", argv[i]);
        return NULL;
      }
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
