#define NUMBER_OF_ARGUMENTS 8

#define LABEL_LEN 128
#define ROW_LEN 1024
#define PATH_LEN 1024
#define STR_LEN 128
#define CMD_LEN 1024
#define TIME_LEN 32
#define METHOD_LEN 32

#define STR_ALERT "alert"

#define STR_METHOD_ALERT "alert"
#define STR_METHOD_SYSLOG "syslog"
#define STR_METHOD_MAIL "mail"
#define STR_METHOD_TRAP "trap"


#define SUCCESS 0
#define ERR_INTERNAL 1
#define ERR_INVALID_PARAM 2
#define ERR_INVALID_FORMAT 3
#define ERR_OPEN_FILE 4
#define ERR_COMMAND_FAILED 5

int sendalert(char *, float, int, int, char *, char *);
