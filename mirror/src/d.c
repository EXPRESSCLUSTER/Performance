// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdperf.h"
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
	long threshold;
	FILE *fp = NULL;
	int i;

	if (!strcmp (argv[1], "alert"))	{
		/* TODO: NULL check */
        if ((argc != 8)) {
            printf("error\n");
            exit(1);
        }
        else {    
            strcpy (label, argv[2]);
            threshold = atoi (argv[3]);
            times = atoi (argv[4]);
            interval = atoi (argv[5]);
            strcpy (method, argv[6]);
            strcpy (path, argv[7]);
            sendalert (label, threshold, times, interval, method, path);
        }
	}
	else {
		printf ("%d: Invalid parameter.\n", __LINE__);
		return ERR_INVALID_PARAM;
	}

	return 0;
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
	int i;
	int kk, number_line;
	char *date;
    int value;
    int counts = 0;
    char *command = "clplogcmd -m ";
    char str[30];
    char *row_time = NULL;

    sprintf(str,"%s \"%s\" --%s\n",command,label,method);
	printf("label    : %s \n", label);
	printf("threshold: %d \n", threshold);
	printf("times    : %d \n", times);
	printf("interval : %d \n", interval);
	printf("method   : %s \n", method);
	printf("path     : %s \n", path);

	/* find the column */
	fp = fopen(path, "r");
	fgets(tmp, sizeof(tmp), fp);
	token = strtok(tmp, "\",");
	i = 0;
	while (token != NULL) {
	    if (!strcmp(token, label)) {
		/* Bingo! */
            column = i;
//            printf("column: %d\n", column);
            break;
	    }
	    token = strtok(NULL, "\"");
	    i++;
	}
	fclose(fp);  

	while (1) {
        
        
//        printf("a \n");
        /* find the last row*/
        fp = fopen(path, "r");
        number_line = 0;
        while (fgets(tmp, sizeof(tmp), fp) != NULL) {
            number_line++;
        }
        fseek(fp, 0L, SEEK_SET);
        for (i = 0; i < number_line - 1; i++) {
            fgets(tmp, sizeof(tmp), fp);
        }
//        printf("%s\n",tmp);

        /* find the 1rowtime*/
        token = strtok(tmp, "\",");
        if (row_time == NULL) {
//            printf("%s\n",token);
            row_time = (char*)malloc(sizeof(char) * sizeof(token));
            strcpy(row_time, token);
//            printf("rowtime %s\n",row_time);
        }
        else {
//            printf("%s\n",token);
            printf("Current time %s\n",row_time);
            if (!strcmp(token, row_time)) {
                printf("error\n");
                row_time = (char*)malloc(sizeof(char) * sizeof(token));
                strcpy(row_time, token);
                break;
            }
            else {
//                printf("NO REPEAT\n");
                row_time = (char*)malloc(sizeof(char) * sizeof(token));
                strcpy(row_time, token);
            }
        }
        /* find the value*/
        for (i = 0; i <= column; i++) {    
            if (column == i) {
                value = atoi(token);
//                printf("target %d\n",value);
                break;
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
            sprintf(str,"%s \"%s\" --%s\n",command,label,method);
            int res = system(str);
            //3,4
            if(WIFEXITED(res) == 1) {
                //  printf("%d\n",WEXITSTATUS(res));
                if (WEXITSTATUS(res) == 0) {
                    //  printf("success\n");
                    counts = 0;
                }
                else {
                    printf("command not found");
                    exit(1);
                }
            }
            //1,2
            else {
                printf("NOT child process");
                exit(1);
            }
            //  printf("check\n");
            sleep(interval);
        }
        else {
            sleep(interval);
        }
    }
	return 0;
}

