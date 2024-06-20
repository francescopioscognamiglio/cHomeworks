#include <stdio.h> // included to use printing functions
#include <stdlib.h> // included to use the exit states and memory allocation functions
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
      formatPhone(user->officePhone));
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
  printf("Office: %s, %s\t\t", user->officeLocation, formatPhone(user->officePhone));
  printf("Home Phone: %s\n", formatPhone(user->homePhone));
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