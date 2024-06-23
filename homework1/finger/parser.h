#include <pwd.h> // included to use the password file

int addInitialUser(char* userName, finger_t* finger);
int addUser(char* userName, finger_t* finger);

int parseOptions(int argc, char* argv[], finger_t* finger);
int parseUsers(int argc, char* argv[], finger_t* finger);

int allocateNewUser(finger_t* finger);
void saveUniqueUser(finger_t* finger, char* userName);

void retrieveGeneralInformation(user_t* user, struct passwd* userPwd, char* userName);
void retrieveLoginRecordInformation(user_t* user, struct passwd* userPwd, struct utmp* loginRecord, char* userName);
void retrieveIdleTime(user_t* user, long time);
void retrieveGecos(user_t* user, struct passwd* userPwd);
