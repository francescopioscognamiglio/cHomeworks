
int addInitialUser(char* userName, finger_t* finger);
int addUser(char* userName, finger_t* finger);

int parseOptions(int argc, char* argv[], finger_t* finger);
int parseUsers(int argc, char* argv[], finger_t* finger);

int retrieveGecos(user_t* user, struct passwd* userPwd);
