#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit states and memory allocation functions
#include <string.h> // included to use string functions
#include <unistd.h> // included to use the username and the user identifier
#include <sys/stat.h> // included to use the stat function
#include <utmp.h> // included to use the utmp file containing login records
#include <pwd.h> // included to use the password file
#include <time.h> // included to use the time functions
#include "structure.h"
#include "parser.h"

int parseOptions(int argc, char* argv[], finger_t* finger) {
  // init format information
  finger->format->isMultiLine = false;
  finger->format->useRealName = true;
  finger->format->showSpecialFiles = true;
  finger->format->foundStyle = false;

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
          finger->format->isMultiLine = true;
          found = true;
          finger->format->foundStyle = true;
        }
        if (argv[i][j] == 'm') {
          finger->format->useRealName = false;
          found = true;
        }
        if (argv[i][j] == 's') {
          if (finger->format->foundStyle == false) {
            finger->format->isMultiLine = false;
          }
          found = true;
          finger->format->foundStyle = true;
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

  return EXIT_SUCCESS;
}

int parseUsers(int argc, char* argv[], finger_t* finger) {
  int status = EXIT_SUCCESS;
  finger->usersSize = 0;

  bool givenOneUser = false;
  // check the provided arguments
  // skip the first argument that is the program name
  for (int i = 1; i < argc; i++) {
    // check if the argument is a user
    if (argv[i][0] != '-') {
      status = addUser(argv[i], finger);
      givenOneUser = true;
      if (status == EXIT_FAILURE) {
        return status;
      }
    }
  }

  // if no user is specified as argument
  if (givenOneUser == false) {
    // retrieve the currently logged user
    char* currentUser = getlogin();
    addInitialUser(currentUser, finger);
  }

  if (finger->format->foundStyle == false) {
    if (finger->usersSize > 0) {
      finger->format->isMultiLine = true;
    } else {
      finger->format->isMultiLine = false;
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

  // retrieve user info
  struct passwd* realUserPwd = getpwuid(getuid());

  char realUserCopy[UT_NAMESIZE];
  strncpy(realUserCopy, realUserPwd->pw_name, UT_NAMESIZE);
  realUserCopy[UT_NAMESIZE-1] = '\0'; // add string termination character
  // point the beginning of the utmp file
  setutent();
  // retrieve the first login record from the utmp file
  struct utmp* loginRecord = getutent();
  // iterate over login records
  bool userFound = false;
  while (loginRecord != NULL) {
    char loginUserCopy[UT_NAMESIZE];
    strncpy(loginUserCopy, loginRecord->ut_user, UT_NAMESIZE);
    loginUserCopy[UT_NAMESIZE-1] = '\0'; // add string termination character
    // print information only of given users
    if ((strcasecmp(loginUserCopy, userName) == 0) &&
        (finger->format->useRealName == false || strcasecmp(realUserCopy, userName) == 0)) {
      userFound = true;

      // allocate memory for user information
      finger->users[finger->usersSize] = (user_t*) malloc(sizeof(user_t));
      if (finger->users[finger->usersSize] == NULL) {
        printf("There was an error allocating the memory.\n");
        return EXIT_FAILURE;
      }

      struct passwd* userPwd = getpwnam(loginUserCopy);

      char terminalSuffix[5];
      strncpy(terminalSuffix, loginRecord->ut_id, 4);
      terminalSuffix[4] = '\0';

      struct stat ttyStat;
      char terminalName[UT_LINESIZE+6];
      strncpy(terminalName, "/dev/", UT_LINESIZE);
      strncat(terminalName, loginRecord->ut_line, UT_LINESIZE);
      terminalName[UT_LINESIZE+5] = '\0';
      stat(terminalName, &ttyStat);

      finger->users[finger->usersSize]->loginName = (char*) calloc(UT_NAMESIZE, sizeof(char));
      strncpy(finger->users[finger->usersSize]->loginName, loginUserCopy, UT_NAMESIZE);

      finger->users[finger->usersSize]->realName = (char*) calloc(UT_NAMESIZE, sizeof(char));
      strncpy(finger->users[finger->usersSize]->realName, realUserCopy, UT_NAMESIZE);

      finger->users[finger->usersSize]->terminalName = (char*) calloc(UT_LINESIZE, sizeof(char));
      strncpy(finger->users[finger->usersSize]->terminalName, loginRecord->ut_line, UT_LINESIZE);

      retrieveIdleTime(finger->users[finger->usersSize], ttyStat.st_atime);

      finger->users[finger->usersSize]->loginDate = loginRecord->ut_tv.tv_sec;

      // FIXME: is there a limit of chars for the home directory?
      finger->users[finger->usersSize]->homeDirectory = (char*) calloc(100, sizeof(char));
      strncpy(finger->users[finger->usersSize]->homeDirectory, userPwd->pw_dir, 100);

      // FIXME: is there a limit of chars for the login shell?
      finger->users[finger->usersSize]->loginShell = (char*) calloc(100, sizeof(char));
      strncpy(finger->users[finger->usersSize]->loginShell, userPwd->pw_shell, 100);

      finger->users[finger->usersSize]->terminalSuffix = (char*) calloc(5, sizeof(char));
      strncpy(finger->users[finger->usersSize]->terminalSuffix, loginRecord->ut_id, 4);
      terminalSuffix[4] = '\0'; // manually set the string termination character

      retrieveGecos(finger->users[finger->usersSize], userPwd);

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

      finger->usersSize++;
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

  return EXIT_SUCCESS;
}

void retrieveIdleTime(user_t* user, long accessTime) {
  time_t now;
  time(&now);
  if (time > 0) {
    long idleTime = now - accessTime;
    if (idleTime > 0) {
      int idleTimeHours = idleTime / 3600;
      int idleTimeAllMinutes = idleTime / 60;
      int idleTimeMinutes = idleTimeAllMinutes % 60;
      int idleTimeSeconds = idleTime % 60;
      user->idleTime = (idletime_t*) malloc(sizeof(idletime_t));
      user->idleTime->hours = idleTimeHours;
      user->idleTime->minutes = idleTimeMinutes;
      user->idleTime->seconds = idleTimeSeconds;
    }
  }
}

void retrieveGecos(user_t* user, struct passwd* userPwd) {
  // The GECOS information can be used to retrieve:
  // - the office location
  // - the office phone
  // - the home phone
  user->officeLocation = NULL;
  user->officePhone = NULL;
  user->homePhone = NULL;
  int start = 0, offset = 0, i = -1;
  int step = 0;
  do {
    i++;
    if (userPwd->pw_gecos[i] != ',' && userPwd->pw_gecos[i] != '\0') {
      offset++;
    } else {
      char* gecosSingleParameter = (char*) calloc(offset+1, sizeof(char));
      memcpy(gecosSingleParameter, &userPwd->pw_gecos[start], offset);
      gecosSingleParameter[offset+1] = '\0';
      switch (step) {
        case 1:
          user->officeLocation = (char*) calloc(20, sizeof(char));
          strncpy(user->officeLocation, gecosSingleParameter, 20);
          break;
        case 2:
          user->officePhone = (char*) calloc(20, sizeof(char));
          strncpy(user->officePhone, gecosSingleParameter, 20);
          break;
        case 3:
          user->homePhone = (char*) calloc(20, sizeof(char));
          strncpy(user->homePhone, gecosSingleParameter, 20);
          break;
      }
      free(gecosSingleParameter);
      step++;
      start = i+1;
      offset = 0;
    }
  } while (userPwd->pw_gecos[i] != '\0');
}
