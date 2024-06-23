#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit states and memory allocation functions
#include <string.h> // included to use string functions
#include <time.h> // included to use the time functions
#include <unistd.h> // included to use the access and stat functions
#include <sys/stat.h> // included to use the stat function
#include <dirent.h> // included to open directory and read its content
#include "structure.h"
#include "printer.h"

#define FORWARD_FILE_NAME ".forward"
#define PGPKEY_FILE_NAME ".pgpkey"
#define PLAN_FILE_NAME ".plan"
#define PROJECT_FILE_NAME ".project"
#define MAIL_DIRECTORY_NAME "/var/mail"

int print(finger_t* finger) {
  // iterate over users
  for (int i = 0; i < finger->usersSize; i++) {

    if (finger->format->isMultiLine == true) {
      printMultipleLines(finger->users[i], finger->format);
    } else {
      printSingleLine(finger->users[i], finger->format);
    }
  }

  return EXIT_SUCCESS;
}

int printSingleLine(user_t* user, format_t* format) {
  // print information on single line
  char loginTimeShort[20];
  time_t ltime = user->loginDate;
  struct tm* timeinfo = localtime(&ltime);

  char* timeFormat = "%b %d %R";
  strftime(loginTimeShort, sizeof(loginTimeShort), timeFormat, timeinfo);

  printf("Login\tName\tTty\tIdle\tLogin Time\tOffice\tOffice Phone\n");
  printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
      user->loginName,
      user->realName,
      user->terminalName,
      formatIdleTime(user->idleTime),
      loginTimeShort,
      user->officeLocation,
      formatPhone(user->officePhone));

  return EXIT_SUCCESS;
}

int printMultipleLines(user_t* user, format_t* format) {
  // print information on multiple lines
  char loginTimeLong[25];
  time_t ltime = user->loginDate;
  struct tm* timeinfo = localtime(&ltime);

  char* timeFormatLong = "%a %b %d %R (%Z)";
  strftime(loginTimeLong, sizeof(loginTimeLong), timeFormatLong, timeinfo);

  printf("Login: %s\t\t", user->loginName);
  printf("Name: %s\n", user->realName);
  printf("Directory: %s\t\t", user->homeDirectory);
  printf("Shell: %s\n", user->loginShell);
  printf("Office: %s, %s\t\t", user->officeLocation, formatPhone(user->officePhone));
  printf("Home Phone: %s\n", formatPhone(user->homePhone));
  printf("On since %s on %s from %s\n", loginTimeLong,
      user->terminalName, user->terminalSuffix);
  printIdleTime(user->idleTime);

  if (format->showSpecialFiles == true) {
    if (printSpecialFiles(user->realName, user->homeDirectory) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

char* formatPhone(char* phone) {
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

char* formatIdleTime(idletime_t* idleTime) {
  char* formattedIdleTime = calloc(7, sizeof(char));
  if (idleTime->hours != 0) {
    sprintf(formattedIdleTime, "%d:%d", idleTime->hours, idleTime->minutes);
  } else {
    sprintf(formattedIdleTime, "%d:%d", idleTime->minutes, idleTime->seconds);
  }
  formattedIdleTime[6] = '\0';
  return formattedIdleTime;
}

void printIdleTime(idletime_t* idleTime) {
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
