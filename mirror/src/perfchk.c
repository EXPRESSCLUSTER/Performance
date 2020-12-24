#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "perfchk.h"

int main (
    int argc, 
    char **argv
    )
{
    /* Define variables */
    char label[LABEL_LEN];
    char method[METHOD_LEN];   
    char path[PATH_LEN];
    int times = 0;
    int interval = 0;
    float threshold = 0;
    FILE *fp = NULL;
    int i = 0;
    int ret = 0;

    /* Initialize */
    memset(label, '\0', sizeof(label));
    memset(method, '\0', sizeof(method));
    memset(path, '\0', sizeof(path));

    /* Check arguments */
    if((argc == NUMBER_OF_ARGUMENTS)){
        if(!strcmp(argv[1], STR_ALERT)){
            strncpy(label, argv[2], LABEL_LEN);
            threshold = atof(argv[3]);
            times = atoi(argv[4]);
            interval = atoi(argv[5]);
            strncpy(method, argv[6], METHOD_LEN);
            strncpy(path, argv[7], PATH_LEN);
           
            /* Check arguments */
            if(threshold <= 0 || times <= 0 || interval <= 0) {
                printf("%d: Invalid parameter (threshold:%f, times:%d, interval:%d).\n", __LINE__, threshold, times, interval);                
                return ERR_INVALID_PARAM;
            }
            if(strcmp(method, STR_METHOD_SYSLOG) && strcmp(method, STR_METHOD_ALERT) && strcmp(method, STR_METHOD_MAIL) && strcmp(method, STR_METHOD_TRAP)) {
                printf("%d: Invalid parameter (method:%s).\n", __LINE__, method);
                return ERR_INVALID_PARAM;
            }
            
            /* Send alert */
            ret = sendalert(label, threshold, times, interval, method, path);
            if(!ret) {
                printf("%d: sendalert() failed (ret:%d).\n", __LINE__, ret);
                return ret;
            }
        }
        else{
            printf("%d: 1st argument is invalid (%s).\n", __LINE__, argv[1]);
            return ERR_INVALID_PARAM;
        }
    }
    else {
        printf("%d: The number of arguments is invalid (%d).\n", __LINE__, argc);
        return ERR_INVALID_PARAM;
    }
    return SUCCESS;
}


int sendalert (
    char *label,
    float threshold,
    int times, 
    int interval, 
    char *method,
    char *path
    )
{
    char *token = NULL;
    char tmp[ROW_LEN];
    int column = 0;
    FILE *fp = NULL;
    int i = 0;
    int line_number = 0;
    float value = 0;
    int counts = 0;
    char *command = "clplogcmd -m ";
    char str[STR_LEN];
    char pre_time[TIME_LEN];
    char cmd_line[CMD_LEN];
    int ret = 0;
    
    /* Initialize */
    memset(tmp, '\0', sizeof(tmp));
    memset(str, '\0', sizeof(str));
    memset(pre_time, '\0', sizeof(pre_time));
    memset(cmd_line, '\0', sizeof(cmd_line));

    /* Output arguments */
    printf("label    : %s \n", label);
    printf("threshold: %f \n", threshold);
    printf("times    : %d \n", times);
    printf("interval : %d \n", interval);
    printf("method   : %s \n", method);
    printf("path     : %s \n", path);

    /* find the column */
    fp = fopen(path, "r");
    if(fp == NULL) {
        printf("%d: File not found (%s).\n", __LINE__, path);
        return ERR_OPEN_FILE;
    }
    else {
        fgets(tmp, sizeof(tmp), fp);
        fclose(fp);
        token = strtok(tmp, "\"");
        while(token != NULL) {
            if(!strcmp(token, label)) {
                column = i;
                break;
            }
            else{
                token = strtok(NULL, "\"");
                i++;
            }
        }
        if(token == NULL) {
            printf("%d: Invalid format.\n", __LINE__);
            return ERR_INVALID_FORMAT;
        }
    }
   
    while (1) {
        /* Find the line number & get the last line */
        fp = fopen(path, "r");
        if(fp == NULL) {
            printf("%d: File not found (%s).\n", __LINE__, path);
            return ERR_OPEN_FILE;
        }
        else{
            line_number = 0;
            while(fgets(tmp, sizeof(tmp), fp) != NULL) {
                line_number++;
            }
            if(line_number <= 1) {
                printf("%d: Invalid format (%d).\n", __LINE__, line_number);
                fclose(fp);
                goto loop;
            }
            fseek(fp, 0L, SEEK_SET);
            for(i = 0; i < line_number - 1; i++) {
                fgets(tmp, sizeof(tmp), fp);
            }
            printf("%s\n",tmp);
        }
        fclose(fp);
        
        /* Compare cur_time to pre_time */
        token = strtok(tmp, "\"");
        if(strlen(pre_time) == 0) {
            strncpy(pre_time, token, TIME_LEN);
        }
        else{
            if(!strcmp(token, pre_time)) {
                printf("%d: Find the same line.\n", __LINE__);
                goto loop;
            }
            else{
                strncpy(pre_time, token, TIME_LEN);
            }
        }

        /* Find the value */
        for(i = 0; i <= column; i++) {
            if(column == i) {
                value = atof(token);
                break;
            }
            else{
                token = strtok(NULL, "\"");
            }
        }
 
        /* Check value */
        if(threshold <= value) {
            counts++;
        }
        else{
            counts = 0;
        }

        /* Over threshold */
        if(times == counts) {
            sprintf(str,"Over threshold (label:%s, threshold:%f, times:%d).", label, threshold, times);
            sprintf(cmd_line,"%s \"%s\" --%s > /dev/null 2>&1", command, str, method);
            ret = system(cmd_line);

            /* Run command */
            if(WIFEXITED(ret)) {
                if(WEXITSTATUS(ret) == 0) {
                    printf("%d: Command success. \n", __LINE__);
                }
                else{
                    printf("%d: Command failed (%d).\n",__LINE__, ret);
                    return ERR_COMMAND_FAILED;
                }
            }
            else{
                printf("%d: system() failed (%d).\n",__LINE__, WIFEXITED(ret));
                return ERR_INTERNAL;
            }
        }
loop:
        sleep(interval);
    }
    return SUCCESS;
}
