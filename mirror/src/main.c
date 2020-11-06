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

	/* initialize variables */
	/*
	 * Expamle:
	 * times = 0;
	 * threshold = 0;
	 */

	if (!strcmp (argv[1], "alert"))	{
		/* TODO: NULL check */
		strcpy (label, argv[2]);
		threshold = atoi (argv[3]);
		times = atoi (argv[4]);
		interval = atoi (argv[5]);
		strcpy (method, argv[6]);
		strcpy (path, argv[7]);
		sendalert (label, threshold, times, interval, method, path);
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
            printf("column: %d\n", column);
            break;
	    }
	    token = strtok(NULL, "\"");
	    i++;
	}
	fclose(fp);  

	while (1) {
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
        printf("%s\n",tmp);

        token = strtok(tmp, "\",");
	    for (i = 0; i <= column; i++) {
            if (column == i) {
                value = atoi(token);
                printf("target %d\n",value);
                break;
            }
            else {
                token = strtok(NULL, "\",");
            }
        }
        fclose(fp);
        
        /* find the last row 
        number_line = 0;
         while (fgets(tmp, sizeof(tmp), fp) != NULL) {
             number_line++;
         }
         fseek(fp, 0L, SEEK_SET);
         for (i = 0; i < number_line - 1; i++) {
             fgets(tmp, sizeof(tmp), fp);
        }*/

               
	    /*find times*/
        token = strtok(tmp, "\",");
	    printf("test %d\n",threshold);
        if (threshold <= value) {
            printf("OK \n");
            counts++;
        }
        else {
            /*printf("Nothing \n");*/
            counts=0;
        }
        printf("%d\n",counts);

        /* threhold = value*/
        if (times = counts) {
            break;
        }
        else {
            sleep(interval);
        }
    }
	return 0;
}
