#define NUMBER_OF_ARGUMENTS 8

#define LABEL_LEN 128
#define ROW_LEN 2048
#define PATH_LEN 1024
#define STR_LEN 1024
#define TIME_LEN 32

#define NORMAL_END 0
#define ERR_INVALID_PARAM 1
#define ERR_BLANK_LINE 1
#define ERR_COMMAND_FAILID 1

int sendalert(char *, float, int, int, char *, char *);
