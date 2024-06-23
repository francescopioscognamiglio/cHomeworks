
int print(finger_t* finger);

int printSingleLine(finger_t* finger);
int printSingleLineHeader(user_t* user, format_t* format);
int printSingleLineBody(user_t* user, format_t* format);

int printMultipleLines(finger_t* finger);
int printMultipleLinesHeader(user_t* user, format_t* format);
int printMultipleLinesBody(user_t* user, format_t* format);
int printMultipleLinesFooter(user_t* user, format_t* format);

char* formatTerminalName(char* terminalName);
char* formatIdleTime(idletime_t* idleTime);
char* formatLoginTime(int loginDate);
char* formatPhone(char* phone);
char* buildPath(char* homeDirectory, char* fileName);

void printGecos(user_t* user);
void printIdleTime(idletime_t* idleTime);
int printSpecialFiles(char* userName, char* homeDirectory);
int printForwardFile(char* homeDirectory, char* fileName);
int printMailStatus(char* mailDirectory, char* userName);
int printPgpKeyFile(char* homeDirectory, char* fileName);
int printProjectFile(char* homeDirectory, char* fileName);
int printPlanFile(char* homeDirectory, char* fileName);
int printLines(char* fileName);
