// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "perfchk.h"
#include <unistd.h>

int main (
    int argc, 
    char **argv
    )
{
    /* define variables */
    char label[LABEL_LEN];
    char method[LABEL_LEN];   
    char path[PATH_LEN];
    int times = 0;
    int interval = 0;
    float threshold = 0;
    FILE *fp = NULL;
    int i = 0;
    int ret = 0;

    /*Initialize*/
    memset(label,0,sizeof(label));
    memset(method,0,sizeof(method));
    memset(path,0,sizeof(path));

    /*Check arguments*/
    if((argc == NUMBER_OF_ARGUMENTS)){
        if(!strcmp(argv[1], "alert")){
            strncpy(label, argv[2],LABEL_LEN);
            threshold = atof(argv[3]);
            times = atoi(argv[4]);
            interval = atoi(argv[5]);
            strncpy(method, argv[6],LABEL_LEN);
            strncpy(path, argv[7],PATH_LEN);
           
            if(threshold <= 0 || times <= 0 || interval <= 0 ) {
                printf("%d: Invalid parameter (threshold:%f, times:%d, interval:%d).\n", __LINE__,threshold,times,interval);                
                return ERR_INVALID_PARAM;
            }
            if(strcmp(method,"syslog") && strcmp(method,"alert") && strcmp(method,"mail") && strcmp(method,"trap")) {
                printf("%d: Invalid parameter (method:%s).\n", __LINE__, method);
                return ERR_INVALID_PARAM;
            }
            ret = sendalert(label, threshold, times, interval, method, path);
            if(!ret) {
                printf("%d: sendalert failed (ret:%d).\n", __LINE__, ret);
                return ret;
            }
        }
        else{
            printf("%d: 1st arguments (%s).\n", __LINE__, argv[1]);
            return ERR_INVALID_PARAM;
        }
    }
    else {
        printf("%d: Not enough arguments (%d).\n", __LINE__, argc);
        return ERR_INVALID_PARAM;
    }
    return SUCCESS;
}


int
sendalert
(
    char *label,
    float threshold, int times, int interval, char *method, char *path)
{
    char *token = NULL;
    char tmp[ROW_LEN];
    int column = 0;
    FILE *fp = NULL;
    int i = 0;
    int number_line=0;
    float value=0;
    int counts = 0;
    char *command = "clplogcmd -m ";
    char str[STR_LEN];
    char pre_time[TIME_LEN];
    char cmd_line[CMD_LEN];
    int ret=0;
    
    /*Initialize*/
    memset(tmp, 0, sizeof(tmp));
    memset(str, 0, sizeof(str));
    memset(pre_time, 0, sizeof(pre_time));
    memset(cmd_line, 0, sizeof(cmd_line));

    /*Output arguments*/
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
        return ERR_INVALID_PARAM;
    }
    else {
        fgets(tmp, sizeof(tmp), fp);
        token = strtok(tmp, "\",");
        if(token == NULL) {
            printf("%d: Wrong format.\n", __LINE__);
            fclose(fp);
            return ERR_NOT_FOUND;
        }
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
    }
    fclose(fp);  
   
    /*infinite loop*/
    while (1) {
        fp = fopen(path, "r");
        if(fp == NULL) {
            printf("%d: File not found (%s).\n", __LINE__, path);
            return ERR_INVALID_PARAM;
        }
        else{
            number_line = 0;
            while(fgets(tmp, sizeof(tmp), fp) != NULL) {
                number_line++;
            }
            if(number_line <= 1) {
                printf("%d: Wrong format (%d).\n", __LINE__, number_line);
                fclose(fp);
                goto loop;
            }
            fseek(fp, 0L, SEEK_SET);
            for(i = 0; i < number_line - 1; i++) {
                fgets(tmp, sizeof(tmp), fp);
            }
            printf("%s\n",tmp);
        }
        /* compare cur_time to pre_time */
        token = strtok(tmp, "\",");
        if(strlen(pre_time) == 0) {
            strncpy(pre_time, token, TIME_LEN);
        }
        else{
            if(!strcmp(token, pre_time)) {
                printf("Find the same line.\n", __LINE__);
                fclose(fp);
                goto loop;
            }
            else{
                strncpy(pre_time, token, TIME_LEN);
            }
        }

        /* find the value */
        for(i = 0; i <= column; i++) {
            if(column == i) {
                value = atof(token);
                break;
            }
            else{
                token = strtok(NULL, "\",");
            }
        }
        fclose(fp); 
 
        /* find times */
        if(threshold <= value) {
            counts++;
        }
        else{
            counts = 0;
        }

        /* Over threshold */
        if(times == counts) {
            sprintf(str,"Over threshold (label:%s, threshold:%f, times:%d).\n", label, threshold,  times);
            sprintf(cmd_line,"%s \"%s\"  --%s\n", command, str, method);
            ret = system(cmd_line);

            /* Error handling of system() */
            if(WIFEXITED(ret)) {
                if(WEXITSTATUS(ret) == 0) {
                    printf("%d: command success. \n", __LINE__);
                }
                else{
                    printf("%d: command failed (%d).\n",__LINE__, ret);
                    fclose(fp);
                    return ERR_COMMAND_FAILED;
                }
            }
            else{
                printf("%d: system() failed (%d).\n",__LINE__, WIFEXITED(ret));
                fclose(fp);
                return ERR_INTERNAL;
            }
        }
loop:
        sleep(interval);
    }
    return SUCCESS;
}
