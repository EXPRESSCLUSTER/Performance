#define LABEL_LEN 128
#define ROW_LEN 2048
#define PATH_LEN 1024
#define ERR_INVALID_PARAM 1

int sendalert(char* label, int threshold, int times, int interval, char* method, char* path);
