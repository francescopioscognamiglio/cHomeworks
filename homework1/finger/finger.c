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
        struct passwd* realUserPwd = getpwuid(getuid());

        char loginTime[25];
        time_t time = loginRecord->ut_tv.tv_sec;
        struct tm* timeinfo = localtime(&time);

        char* timeFormat = "%b %d %R";
        strftime(loginTime, sizeof(loginTime), timeFormat, timeinfo);

        printf("Printing information on single line ...\n");
        printf("Login\tName\tTty\tIdle\tLogin Time\tOffice\tOffice Phone\n");
        printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n", finger->users[i],
            realUserPwd->pw_name, loginRecord->ut_line,
            "FIXME", loginTime,
            "FIXME", "FIXME");


        char terminalSuffix[5];
        strncpy(terminalSuffix, loginRecord->ut_id, 4);
        terminalSuffix[4] = '\0';

        struct passwd* currentUserPwd = getpwnam(finger->users[i]);

        char* timeFormatLong = "%a %b %d %R (%Z)";
        strftime(loginTime, sizeof(loginTime), timeFormatLong, timeinfo);

        printf("\n\nPrinting information on multiple lines ...\n");
        printf("Login: %s\t\t", finger->users[i]);
        printf("Name: %s\n", realUserPwd->pw_name);
        printf("Directory: %s\t\t", currentUserPwd->pw_dir);
        printf("Shell: %s\n", currentUserPwd->pw_shell);
        printf("On since %s on %s from %s\n", loginTime, loginRecord->ut_line, terminalSuffix);
        printf("\t%s idle\n", "FIXME");
        printf("%s\n", "FIXME No mail.");
        printf("%s\n", "FIXME No Plan.");
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
