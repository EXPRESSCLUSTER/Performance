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
    int times;
    int interval;
    float threshold;
    FILE *fp = NULL;
    int i;
    int ret;

    if ((argc == NUMBER_OF_ARGUMENTS)){
        if (!strcmp (argv[1], "alert")) {    
            strncpy(label, argv[2],LABEL_LEN);
            threshold = atof(argv[3]);
            times = atoi(argv[4]);
            interval = atoi(argv[5]);
            strncpy(method, argv[6],LABEL_LEN);
            strncpy(path, argv[7],PATH_LEN);
            // if (||)
            sendalert(label, threshold, times, interval, method, path);
            ret = sendalert(label, threshold, times, interval, method, path);
            if (!ret) {
                printf("sendalert failed (ret: %d).\n", ret);
                return ERR_INVALID_PARAM;
            }
        }
        else {
            printf("%d: error\n",__LINE__);
            return ERR_INVALID_PARAM;
        }
    }
    else {
        printf ("%d: Invalid parameter.\n", __LINE__);
        return ERR_INVALID_PARAM;
    }

    return NORMAL_END;
}


int
sendalert
(
    char *label,
    int threshold, int times, int interval, char *method, char *path)
{
    char *token = NULL;
    char tmp[ROW_LEN];
    int column;
    FILE *fp = NULL;
    int i = 0;
    int number_line;
    char *date;
    int value;
    int counts = 0;
    char *command = "clplogcmd -m ";
    char str[STR_LEN];
    char *previous_time = NULL;

    sprintf(str,"%s \"%s\" --%s\n",command,label,method);
    printf("label    : %s \n", label);
    printf("threshold: %d \n", threshold);
    printf("times    : %d \n", times);
    printf("interval : %d \n", interval);
    printf("method   : %s \n", method);
    printf("path     : %s \n", path);

    /* find the column */
    fp = fopen(path, "r");
    if(fp == NULL) {
        printf ("%d: File not found.\n", __LINE__);
        return ERR_INVALID_PARAM;
    }
    else {
        fgets(tmp, sizeof(tmp), fp);
        token = strtok(tmp, "\",");
        while (token != NULL) {
            if (!strcmp(token, label)) {
                /* Bingo! */
                column = i;
            }
            else {
            token = strtok(NULL, "\"");
            i++;
            }
        }
    }
    fclose(fp);  

    while (1) {        
//        printf("a \n");
        /* find the last row*/
        fp = fopen(path, "r");
        if(fp == NULL) {
            printf ("%d: File not found.\n", __LINE__);
            return ERR_INVALID_PARAM;
        }
        else {
            number_line = 0;
            while (fgets(tmp, sizeof(tmp), fp) != NULL) {
                number_line++;
            }
            fseek(fp, 0L, SEEK_SET);
            for (i = 0; i < number_line - 1; i++) {
                fgets(tmp, sizeof(tmp), fp);
            }
            printf("%s\n",tmp);
        }
        /* find the 1rowtime*/
        token = strtok(tmp, "\",");
        if (previous_time == NULL) {
            printf("%s\n",token);
            previous_time = (char*)malloc(sizeof(char) * sizeof(token));
            strcpy(previous_time, token);
            free(previous_time);
//         
        }
        else {
            printf("%s\n",token);
            printf("Current time %s\n",previous_time);
            if (!strcmp(token, previous_time)) {
                printf("%d: error\n",__LINE__);
//                row_time = (char*)malloc(sizeof(char) * sizeof(token));
//                strcpy(row_time, token);
                break;
            }
            else {
//                printf("NO REPEAT\n");
                previous_time = (char*)malloc(sizeof(char) * sizeof(token));
                strcpy(previous_time, token);
                free(previous_time);
            }
        }
        /* find the value*/
        for (i = 0; i <= column; i++) {    
            if (column == i) {
                value = atoi(token);
//                printf("target %d\n",value);
//                break;
            }
            else {
                token = strtok(NULL, "\",");
            }
        }
        fclose(fp); 

               
        /*find times*/
        token = strtok(tmp, "\",");
        if (threshold <= value) {
//            printf("OK \n");
            counts++;
        }
        else {
            /*printf("Nothing \n");*/
            counts=0;
        }
//        printf("%d\n",counts);

        /* threhold = value*/
        if (times == counts) {
            printf("This %s has crossed the threshold %d times \n ",label ,times );
            sprintf(str,"%s \" Thid %s has crossed the threshold %d times \" --%s\n",command,label,times,method);
            int res = system(str);
            
            if(WIFEXITED(res)) {
                //  printf("%d\n",WEXITSTATUS(res));
                if (WEXITSTATUS(res) == 0) {
                    //  printf("success\n");
                    //  counts = 0;
                    printf("command succes");
                }
                else {
                    printf("%d: command failid",__LINE__);
                    return ERR_INVALID_PARAM;
                }
            }
            
            else {
                printf("%d: NOT child process",__LINE__);
                return ERR_INVALID_PARAM;
            }
            //  printf("check\n");
            sleep(interval);
        }
        else {
            sleep(interval);
        }
        return NORMAL_END;
    }     
}
