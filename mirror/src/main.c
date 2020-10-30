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

	if (!strcmp (argv[1], "alert"))
	{
		/* TODO: NULL check */
		strcpy (label, argv[2]);
		threshold = atoi (argv[3]);
		times = atoi (argv[4]);
		interval = atoi (argv[5]);
		strcpy (method, argv[6]);
		strcpy (path, argv[7]);
		sendalert (label, threshold, times, interval, method, path);
	}
	else
	{
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
	int kk, n_line;
	char *date;
 	double a,b,c; 

	printf ("label    : %s \n", label);
	printf ("threshold: %d \n", threshold);
	printf ("times    : %d \n", times);
	printf ("interval : %d \n", interval);
	printf ("method   : %s \n", method);
	printf ("path     : %s \n", path);

	/* find the column */
	fp = fopen (path, "r");
	fgets (tmp, sizeof (tmp), fp);
	token = strtok (tmp, "\",");
	i = 0;
	while (token != NULL)
	{
		if (!strcmp (token, label))
		{
			/* Bingo! */
			column = i;
			printf ("column: %d\n", column);
			break;
		}
		token = strtok (NULL, "\"");
		i++;
	}

	/* find the last row */
	n_line = 0;
	while(fgets(tmp, sizeof (tmp), fp)!=NULL)
	{
		n_line++;
	}
	fclose(fp);
	fp = fopen(path,"r"); 
	for (kk = 0; kk < n_line - 1; kk++)
	{
		fgets(tmp, sizeof (tmp), fp);
	}
/*
#if 0
  fscanf(fp, "%s %lf %lf %lf\n", &date, &a, &b, &c);
  printf("date %s, a %lf, b %lf, c %lf", date, a, b, c);
#endif
*/
	printf("%s\n", tmp);
	/*	printf("%s\n", token);*/
	fclose(fp);  
	while (1)
	{
		/* find the column on the last row */
	  /*	  fp = fopen (path, "r");
		  fgets (tmp, sizeof (tmp), fp);*/
          i=0;
	  /* printf("%s\n",token);*/
	  while (i <= column)
	    {
	      if (column == i)
	      {
		/*printf("%d\n",i);
		  puts(token);*/
		printf("target %s\n",token);
		break;
	      }
	      token = strtok (NULL, "\",");
	      i++;
	   } 
	  /*  if(token < threshold)
	    {
	      print("OK\n");
	      }*/
		sleep(interval);
	}
	return 0;
}
