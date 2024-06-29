
typedef struct {
  char* address;
  int port;
  char* rootDirectory;
} options_t;

options_t* parseOptions(int argc, char* argv[]);
