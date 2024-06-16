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

int addInitialUser(char* userName, finger_t* finger) {
  finger->usersSize = 0;
  return addUser(userName, finger);
}

int addUser(char* userName, finger_t* finger) {
  if (finger->usersSize == 0) {
    // allocate memory for the first user
    finger->users = (user_t**) malloc(sizeof(user_t*));
  } else if (finger->usersSize > 0) {
    // re-allocate memory for new users
    finger->users = (user_t**) realloc(finger->users, (finger->usersSize+1) * sizeof(user_t*));
  }

  if (finger->users == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }

  // allocate memory for user information
  finger->users[finger->usersSize] = (user_t*) malloc(sizeof(user_t));
  if (finger->users[finger->usersSize] == NULL) {
    printf("There was an error allocating the memory.\n");
    return EXIT_FAILURE;
  }


  char aux[UT_NAMESIZE];
  // point the beginning of the utmp file
  setutent();
  // retrieve the first login record from the utmp file
  struct utmp* loginRecord = getutent();
  // iterate over login records
  bool userFound = false;
  while(loginRecord != NULL) {
    // use strncpy to add termination character
    strncpy(aux, loginRecord->ut_user, UT_NAMESIZE);
    aux[UT_NAMESIZE-1] = '\0';
    // print information only of given users
    if (strcmp(aux, userName) == 0) {
      userFound = true;

      struct passwd* realUserPwd = getpwuid(getuid());

      char terminalSuffix[5];
      strncpy(terminalSuffix, loginRecord->ut_id, 4);
      terminalSuffix[4] = '\0';

      struct passwd* currentUserPwd = getpwnam(userName);

      struct stat ttyStat;
      char terminalName[UT_LINESIZE+6];
      strncpy(terminalName, "/dev/", UT_LINESIZE);
      strncat(terminalName, loginRecord->ut_line, UT_LINESIZE);
      terminalName[UT_LINESIZE+5] = '\0';
      stat(terminalName, &ttyStat);
      time_t now;
      time(&now);
      double idleTime = now - ttyStat.st_atime;

      finger->users[finger->usersSize]->loginName = (char*) calloc(UT_NAMESIZE, sizeof(char));
      strncpy(finger->users[finger->usersSize]->loginName, userName, UT_NAMESIZE);

      finger->users[finger->usersSize]->realName = (char*) calloc(UT_NAMESIZE, sizeof(char));
      strncpy(finger->users[finger->usersSize]->realName, realUserPwd->pw_name, UT_NAMESIZE);

      finger->users[finger->usersSize]->terminalName = (char*) calloc(UT_LINESIZE, sizeof(char));
      strncpy(finger->users[finger->usersSize]->terminalName, loginRecord->ut_line, UT_LINESIZE);

      finger->users[finger->usersSize]->idleTimeHours = (int) idleTime / 3600;
      finger->users[finger->usersSize]->idleTimeMinutes = (int) (idleTime / 60) % 60;

      finger->users[finger->usersSize]->loginDate = loginRecord->ut_tv.tv_sec;

      // FIXME: manage office location
      finger->users[finger->usersSize]->officeLocation = (char*) calloc(20, sizeof(char));
      strncpy(finger->users[finger->usersSize]->officeLocation, "FIXME", 20);

      // FIXME: manage office phone number
      finger->users[finger->usersSize]->officePhoneNumber = (char*) calloc(20, sizeof(char));
      strncpy(finger->users[finger->usersSize]->officePhoneNumber, "FIXME", 20);

      // FIXME: is there a limit of chars for the home directory?
      finger->users[finger->usersSize]->homeDirectory = (char*) calloc(200, sizeof(char));
      strncpy(finger->users[finger->usersSize]->homeDirectory, currentUserPwd->pw_dir, 200);

      // FIXME: is there a limit of chars for the login shell?
      finger->users[finger->usersSize]->loginShell = (char*) calloc(200, sizeof(char));
      strncpy(finger->users[finger->usersSize]->loginShell, currentUserPwd->pw_shell, 200);

      finger->users[finger->usersSize]->terminalSuffix = (char*) calloc(5, sizeof(char));
      strncpy(finger->users[finger->usersSize]->terminalSuffix, loginRecord->ut_id, 4);
      terminalSuffix[4] = '\0'; // manually set the string termination character

      // FIXME: manage mail file
      finger->users[finger->usersSize]->mail = (char*) calloc(20, sizeof(char));
      strncpy(finger->users[finger->usersSize]->mail, "FIXME", 20);

      // FIXME: manage plan file
      finger->users[finger->usersSize]->plan = (char*) calloc(20, sizeof(char));
      strncpy(finger->users[finger->usersSize]->plan, "FIXME", 20);

      // FIXME: manage project file
      finger->users[finger->usersSize]->project = (char*) calloc(20, sizeof(char));
      strncpy(finger->users[finger->usersSize]->project, "FIXME", 20);

      // FIXME: manage pgpkey file
      finger->users[finger->usersSize]->pgpkey = (char*) calloc(20, sizeof(char));
      strncpy(finger->users[finger->usersSize]->pgpkey, "FIXME", 20);

      // FIXME: manage forward file
      finger->users[finger->usersSize]->forward = (char*) calloc(20, sizeof(char));
      strncpy(finger->users[finger->usersSize]->forward, "FIXME", 20);
    }

    // retrieve the next login record
    loginRecord = getutent();
  }

  // close the utmp file
  endutent();

  if (userFound == false) {
    printf("finger: %s: no such user.\n", userName);
    return EXIT_SUCCESS;
  }

  finger->usersSize++;
  return EXIT_SUCCESS;
}