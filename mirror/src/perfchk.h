#define NUMBER_OF_ARGUMENTS 8

#define LABEL_LEN 128
#define ROW_LEN 1024
#define PATH_LEN 1024
#define STR_LEN 1024
#define TIME_LEN 32

#define SUCCESS 0
#define ERR_INTERNAL 1
#define ERR_INVALID_PARAM 2
#define ERR_NOT_FOUND 3
#define ERR_COMMAND_FAILID 4

int sendalert(char *, float, int, int, char *, char *);
