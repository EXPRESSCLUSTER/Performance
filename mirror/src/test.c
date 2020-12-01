#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mdperf.h"
#include <unistd.h>
 
int main(void) {
//    char str1[30] = "Hello ";
//    char str2[] = "World!";
//    strcat(str1,str2);
//    printf("%s\n",str1);
    char *str = "/usr/bin/ls";
    int res = system(str);
    printf("%d\n",WIFEXITED(res));
    printf("%d\n",res);
    
    if (WIFEXITED(res) == 1) {
        printf("%d\n",WEXITSTATUS(res));
    }
    else {
    }
        //      printf("1\n");
   
   
   
        //   print("0\n");
   
        
    

    
    //   switch (res = system(str)) {
//    case 0:
//        printf("ret 0 \n");
//        printf("%d\n",WIFEXITED(res));
        //                  system(str);
//        break;
//    case -1:
//        printf("child process or \n");
//        exit(1);
//    default:
//        printf("system error\n");
//        exit(1);
//    }
    return(0);
}
   
