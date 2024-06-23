
int print(finger_t* finger);
int printSingleLine(user_t* user, format_t* format, bool printHeader);
int printMultipleLines(user_t* user, format_t* format);

char* formatPhone(char* phone);
char* formatIdleTime(idletime_t* idleTime);
char* buildPath(char* homeDirectory, char* fileName);

void printIdleTime(idletime_t* idleTime);
int printSpecialFiles(char* userName, char* homeDirectory);
int printForwardFile(char* homeDirectory, char* fileName);
int printMailStatus(char* mailDirectory, char* userName);
int printPgpKeyFile(char* homeDirectory, char* fileName);
int printProjectFile(char* homeDirectory, char* fileName);
int printPlanFile(char* homeDirectory, char* fileName);
int printLines(char* fileName);
