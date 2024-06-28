#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit states and memory allocation functions
#include <string.h> // included to use string functions
#include <time.h> // included to use the time functions
#include <unistd.h> // included to use the access and stat functions
#include <sys/stat.h> // included to use the stat function
#include <dirent.h> // included to open directory and read its content
#include "structure.h"
#include "printer.h"

int print(finger_t* finger) {
  if (finger->format->isMultiLine == true) {
    printMultipleLines(finger);
  } else {
    printSingleLine(finger);
  }

  return EXIT_SUCCESS;
}

int printMultipleLines(finger_t* finger) {
  // iterate over unique users
  for (int i = 0; i < finger->uniqueUsersSize; i++) {
    user_t* lastUser = (user_t*) malloc(sizeof(user_t));
    // retrieve each logged user
    bool printHeader = true;
    for (int j = 0; j < finger->usersSize; j++) {
      if (strcasecmp(finger->uniqueUsers[i], finger->users[j]->loginName) == 0) {
        if (printHeader == true) {
          // print header
          printMultipleLinesHeader(finger->users[j], finger->format);
          printHeader = false;
        }
        // print body of each logged user
        printMultipleLinesBody(finger->users[j], finger->format);

        // save last user
        lastUser = finger->users[j];
      }
    }
    // print footer
    printMultipleLinesFooter(lastUser, finger->format);
    printf("\n");
  }

  return EXIT_SUCCESS;
}

int printSingleLine(finger_t* finger) {
  // iterate over unique users
  bool printHeader = true;
  for (int i = 0; i < finger->uniqueUsersSize; i++) {
    user_t* lastUser = (user_t*) malloc(sizeof(user_t));
    // retrieve each logged user
    for (int j = 0; j < finger->usersSize; j++) {
      if (strcasecmp(finger->uniqueUsers[i], finger->users[j]->loginName) == 0) {
        if (printHeader == true) {
          // print header
          printSingleLineHeader(finger->users[j], finger->format);
          printHeader = false;
        }
        // print body of each logged user
        printSingleLineBody(finger->users[j], finger->format);

        // save last user
        lastUser = finger->users[j];
      }
    }
  }

  return EXIT_SUCCESS;
}

int printSingleLineHeader(user_t* user, format_t* format) {
  // print header of single line format
  printf("%-15s%-15s%-5s\t%4s\t%-13s%-8s%-10s\n",
      "Login",
      "Name",
      "Tty",
      "Idle",
      "Login Time",
      "Office",
      "Office Phone");

  return EXIT_SUCCESS;
}

int printSingleLineBody(user_t* user, format_t* format) {
  // print body of single line format
  printf("%-15s%-15s%-5s\t%4s\t%-13s%-8s%-10s\n",
      user->loginName,
      user->realName,
      formatTerminalName(user->terminalName),
      formatIdleTime(user->idleTime),
      formatLoginTime(user->loginDate),
      user->officeLocation,
      formatPhone(user->officePhone));

  return EXIT_SUCCESS;
}

int printMultipleLinesHeader(user_t* user, format_t* format) {
  // print header of multiple lines format
  printf("%s: %-32s", "Login", user->loginName);
  printf("Name: %s\n", user->realName);
  printf("%s: %-28s", "Directory", user->homeDirectory);
  printf("Shell: %s\n", user->loginShell);

  printGecos(user);

  return EXIT_SUCCESS;
}

int printMultipleLinesBody(user_t* user, format_t* format) {
  // print body of multiple lines format

  if (user->loginDate <= 0) {
    printf("Never logged in.\n");
    return EXIT_SUCCESS;
  }

  char loginTimeLong[25];
  time_t ltime = user->loginDate;
  struct tm* timeinfo = localtime(&ltime);

  char* timeFormatLong = "%a %b %d %R (%Z)";
  strftime(loginTimeLong, sizeof(loginTimeLong), timeFormatLong, timeinfo);

  printf("On since %s on %s from %s\n", loginTimeLong,
      user->terminalName, user->terminalSuffix);
  printIdleTime(user->idleTime);

  return EXIT_SUCCESS;
}

int printMultipleLinesFooter(user_t* user, format_t* format) {
  // print footer of multiple lines format
  if (format->showSpecialFiles == true) {
    if (printSpecialFiles(user->realName, user->homeDirectory) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

char* formatTerminalName(char* terminalName) {
  if (terminalName == NULL || terminalName[0] == '\0') {
    return "*";
  }

  return terminalName;
}

char* formatIdleTime(idletime_t* idleTime) {
  if (idleTime == NULL ||
      (idleTime->hours == -1 && idleTime->minutes == -1 && idleTime->seconds == -1)) {
    return "*";
  }

  char* formattedIdleTime = calloc(7, sizeof(char));
  if (idleTime->hours != 0) {
    // print hours and minutes
    sprintf(formattedIdleTime, "%d:%d", idleTime->hours, idleTime->minutes);
  } else {
    // print minutes
    sprintf(formattedIdleTime, "%d", idleTime->minutes);
  }
  formattedIdleTime[6] = '\0';
  return formattedIdleTime;
}

char* formatLoginTime(int loginDate) {
  if (loginDate == -1) {
    return "No logins";
  }

  char* loginTimeShort = calloc(20, sizeof(char));
  time_t ltime = loginDate;
  struct tm* timeinfo = localtime(&ltime);

  char* timeFormat = "%b %d %R";
  strftime(loginTimeShort, sizeof(loginTimeShort), timeFormat, timeinfo);

  return loginTimeShort;
}

char* formatPhone(char* phone) {
  if (phone == NULL) {
    return "";
  }

  int i = 0;
  while (phone[i] != '\0') {
    i++;
  }

  // phone numbers format:
  // - eleven digits are printed as '+N-NNN-NNN-NNNN'
  // - ten digits are printed as 'NNN-NNN-NNNN'
  // - seven digits are printed as 'NNN-NNNN'
  // - five digits are printed as 'xN-NNNN'
  // - four digits are printed as 'xNNNN'
  char* formattedPhone = calloc(15, sizeof(char));
  switch (i) {
    case 4:
      sprintf(formattedPhone, "x%s", phone);
      formattedPhone[5] = '\0';
      break;
    case 5:
      sprintf(formattedPhone, "x%c-%c%c%c%c", phone[0],
        phone[1], phone[2], phone[3], phone[4]);
      formattedPhone[7] = '\0';
      break;
    case 7:
      sprintf(formattedPhone, "%c%c%c-%c%c%c%c",
        phone[0], phone[1], phone[2], phone[3],
        phone[4], phone[5], phone[6]);
      formattedPhone[8] = '\0';
      break;
    case 10:
      sprintf(formattedPhone, "%c%c%c-%c%c%c-%c%c%c%c",
        phone[0], phone[1], phone[2],
        phone[3], phone[4], phone[5],
        phone[6], phone[7], phone[8], phone[9]);
      formattedPhone[12] = '\0';
      break;
    case 11:
      sprintf(formattedPhone, "%c-%c%c%c-%c%c%c-%c%c%c%c", phone[0],
        phone[1], phone[2], phone[3],
        phone[4], phone[5], phone[6],
        phone[7], phone[8], phone[9], phone[10]);
      formattedPhone[14] = '\0';
      break;
  }

  return formattedPhone;
}

void printGecos(user_t* user) {
  char office[30] = {0};
  if (user->officeLocation != NULL && user->officeLocation[0] != '\0') {
    strncpy(office, user->officeLocation, 12);
  }
  if (user->officePhone != NULL && user->officePhone[0] != '\0') {
    strncat(office, ", ", 3);
    strncat(office, formatPhone(user->officePhone), 15);
  }

  if (office[0] != '\0') {
    printf("%s: %-31s", "Office", office);
    printf("Home Phone: %s\n", formatPhone(user->homePhone));
  }
}

void printIdleTime(idletime_t* idleTime) {
  if (idleTime == NULL) {
    return;
  }

  if (idleTime->hours != 0) {
    printf("\t%d hours %d minutes idle\n",
      idleTime->hours,
      idleTime->minutes);
  } else {
    printf("\t%d minutes %d seconds idle\n",
      idleTime->minutes,
      idleTime->seconds);
  }
}

int printSpecialFiles(char* userName, char* homeDirectory) {
  // print forward file
  printForwardFile(homeDirectory, FORWARD_FILE_NAME);

  // print mail status
  printMailStatus(MAIL_DIRECTORY_NAME, userName);

  // print pgpkey file
  printPgpKeyFile(homeDirectory, PGPKEY_FILE_NAME);

  // print project file
  printProjectFile(homeDirectory, PROJECT_FILE_NAME);

  // print plan file
  printPlanFile(homeDirectory, PLAN_FILE_NAME);
}

char* buildPath(char* homeDirectory, char* fileName) {
  // build the path to the file
  char* path = calloc(150, sizeof(char));
  strncpy(path, homeDirectory, 100);
  strncat(path, "/", 2);
  strncat(path, fileName, 50);
  return path;
}

int printForwardFile(char* homeDirectory, char* fileName) {
  char* path = buildPath(homeDirectory, fileName);
  if (access(path, R_OK) == 0) {
    // the file exists, print its content
    printf("Mail forwarded to ");
    printLines(path);
  }
}

int printMailStatus(char* mailDirectory, char* userName) {
  if (userName == NULL || userName[0] == '\0') {
    printf("No mail.\n");
    return EXIT_FAILURE;
  }

  char* userMailDirectoryPath = buildPath(mailDirectory, userName);
  DIR* userMailDirectory = opendir(userMailDirectoryPath);
  if (userMailDirectory == NULL) {
    printf("No mail.\n");
    return EXIT_FAILURE;
  }

  // retrieve statistics on mailbox:
  // - the last modification date means the mail creation date
  // - the last access date means the mail read date
  long created = -1, read = -1;
  struct stat* mailboxStat = malloc(sizeof(struct stat));
  stat(userMailDirectoryPath, mailboxStat);
  char* timeFormatLong = "%a %b %d %R %Y (%Z)";
  created = mailboxStat->st_mtime;
  char createdTimeFormatted[30];
  struct tm* createdTimeInfo = localtime(&created);
  strftime(createdTimeFormatted, sizeof(createdTimeFormatted), timeFormatLong, createdTimeInfo);
  read = mailboxStat->st_atime;
  char readTimeFormatted[30];
  struct tm* readTimeInfo = localtime(&read);
  strftime(readTimeFormatted, sizeof(readTimeFormatted), timeFormatLong, readTimeInfo);
  if (created == -1 && read == -1) {
    printf("No mail.\n");
    return EXIT_FAILURE;
  }
  if (created > read) {
    // there is a new mail
    printf("New mail received %s\n", createdTimeFormatted);
    printf("\tUnread since %s\n", readTimeFormatted);
  } else {
    // all the mails have already been read
    printf("Mail last read %s\n", readTimeFormatted);
  }

  return EXIT_SUCCESS;
}

int printPgpKeyFile(char* homeDirectory, char* fileName) {
  char* path = buildPath(homeDirectory, fileName);
  if (access(path, R_OK) == 0) {
    // the file exists, print its content
    printf("PGP key:\n");
    printLines(path);
  }

  return EXIT_SUCCESS;
}

int printProjectFile(char* homeDirectory, char* fileName) {
  char* path = buildPath(homeDirectory, fileName);
  if (access(path, R_OK) == 0) {
    // the file exists, print its content
    printf("Project:\n");
    printLines(path);
  }

  return EXIT_SUCCESS;
}

int printPlanFile(char* homeDirectory, char* fileName) {
  char* path = buildPath(homeDirectory, fileName);
  if (access(path, R_OK) == 0) {
    // the file exists, print its content
    printf("Plan:\n");
    printLines(path);
  } else {
    printf("No Plan.\n");
  }

  return EXIT_SUCCESS;
}

int printLines(char* path) {
  // open the file in read mode
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    printf("There was an error opening the file: %s.\n", path);
    return EXIT_FAILURE;
  }

  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, file)) != -1) {
    // print each line
    printf("%s", line);
  }

  fclose(file);
  if (line != NULL) {
    free(line);
  }

  return EXIT_SUCCESS;
}
