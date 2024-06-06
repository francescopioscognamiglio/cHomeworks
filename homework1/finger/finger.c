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

  // if no user is specified as argument
  if (finger->usersSize <= 0) {
    // retrieve the currently logged user
    char* currentUser = getlogin();
    addInitialUser(currentUser, finger);
  }

  char aux[UT_NAMESIZE];
  struct utmp* loginRecord;
  // iterate over given users
  for (int i = 0; i < finger->usersSize; i++) {
    // retrieve the first login record
    loginRecord = getutent();
    // iterate over login records
    while(loginRecord != NULL) {
      // use strncpy to add termination character
      strncpy(aux, loginRecord->ut_user, UT_NAMESIZE);
      aux[UT_NAMESIZE-1] = '\0';
      // print information only of given users
      if (strcmp(aux, finger->users[i]) == 0) {
        printf("username: %s\n", aux);
        printf("type: %hi\n", loginRecord->ut_type);
        strncpy(aux, loginRecord->ut_id, 4);
        aux[4] = '\0';
        printf("terminal name suffix: %s\n", aux);
        printf("device name of tty: %s\n\n", loginRecord->ut_line);
      }

      // retrieve the next login record
      loginRecord = getutent();
    }
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
