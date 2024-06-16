#include <stdio.h> // included to use printing functions
#include <time.h> // included to use the time functions
#include "structure.h"
#include "printer.h"

int print(finger_t* finger) {
  // iterate over users
  for (int i = 0; i < finger->usersSize; i++) {

    if (finger->format->isMultiLine == true) {
      printMultipleLines(finger->users[i], finger->format);
    } else {
      printSingleLine(finger->users[i], finger->format);
    }
  }
}

int printSingleLine(user_t* user, format_t* format) {
  // print information on single line
  char loginTimeShort[20];
  time_t ltime = user->loginDate;
  struct tm* timeinfo = localtime(&ltime);

  char* timeFormat = "%b %d %R";
  strftime(loginTimeShort, sizeof(loginTimeShort), timeFormat, timeinfo);

  printf("Printing information on single line ...\n");
  printf("Login\tName\tTty\tIdle\tLogin Time\tOffice\tOffice Phone\n");
  printf("%s\t%s\t%s\t%d:%d\t%s\t%s\t%s\n",
      user->loginName,
      user->realName,
      user->terminalName,
      user->idleTimeHours,
      user->idleTimeMinutes,
      loginTimeShort,
      user->officeLocation,
      user->officePhoneNumber);
}

int printMultipleLines(user_t* user, format_t* format) {
  // print information on multiple lines
  char loginTimeLong[25];
  time_t ltime = user->loginDate;
  struct tm* timeinfo = localtime(&ltime);

  char* timeFormatLong = "%a %b %d %R (%Z)";
  strftime(loginTimeLong, sizeof(loginTimeLong), timeFormatLong, timeinfo);

  printf("Printing information on multiple lines ...\n");
  printf("Login: %s\t\t", user->loginName);
  printf("Name: %s\n", user->realName);
  printf("Directory: %s\t\t", user->homeDirectory);
  printf("Shell: %s\n", user->loginShell);
  printf("On since %s on %s from %s\n", loginTimeLong,
      user->terminalName, user->terminalSuffix);
  printf("\t%d hours %d minutes idle\n",
      user->idleTimeHours,
      user->idleTimeMinutes);

  // FIXME: print mail status
  printf("%s\n", user->mail);

  // FIXME: print:
  // - .plan
  // - .project
  // - .pgpkey
  if (format->showSpecialFiles == true) {
    printf("%s\n", user->plan);
  }
}