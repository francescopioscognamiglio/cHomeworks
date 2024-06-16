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

  // iterate over given users
  for (int i = 0; i < finger->usersSize; i++) {
    // print information
    char loginTimeShort[20], loginTimeLong[25];
    time_t ltime = finger->users[i]->loginDate;
    struct tm* timeinfo = localtime(&ltime);

    char* timeFormat = "%b %d %R";
    strftime(loginTimeShort, sizeof(loginTimeShort), timeFormat, timeinfo);

    char* timeFormatLong = "%a %b %d %R (%Z)";
    strftime(loginTimeLong, sizeof(loginTimeLong), timeFormatLong, timeinfo);

    printf("Printing information on single line ...\n");
    printf("Login\tName\tTty\tIdle\tLogin Time\tOffice\tOffice Phone\n");
    printf("%s\t%s\t%s\t%d:%d\t%s\t%s\t%s\n",
        finger->users[i]->loginName,
        finger->users[i]->realName,
        finger->users[i]->terminalName,
        finger->users[i]->idleTimeHours,
        finger->users[i]->idleTimeMinutes,
        loginTimeShort,
        finger->users[i]->officeLocation,
        finger->users[i]->officePhoneNumber);

    printf("Printing information on multiple lines ...\n");
    printf("Login: %s\t\t", finger->users[i]->loginName);
    printf("Name: %s\n", finger->users[i]->realName);
    printf("Directory: %s\t\t", finger->users[i]->homeDirectory);
    printf("Shell: %s\n", finger->users[i]->loginShell);
    printf("On since %s on %s from %s\n", loginTimeLong,
        finger->users[i]->terminalName, finger->users[i]->terminalSuffix);
    printf("\t%d hours %d minutes idle\n",
        finger->users[i]->idleTimeHours,
        finger->users[i]->idleTimeMinutes);
    printf("%s\n", finger->users[i]->mail);
    printf("%s\n", finger->users[i]->plan);
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
  printf("finger: usage: finger [-lmsp] [user ...]\n");
}
