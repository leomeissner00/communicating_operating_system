#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main() {

    char buf[30];
    int k;
    int ret = 1;
    int pid;
    
    while(ret == 1) {
    
        printf(">>>");
        ret = scanf("%s", buf);
        
        if ((strcmp(buf,"exit") == 0) | (ret == EOF)) {
            ret = EOF;
            printf("\n");
        }
        //else {
        pid = fork();
       // }
        
        if (pid == 0) {
            if ((strlen( buf ) > 1)) {
                execlp(buf,buf, (char*)NULL);
                printf("ECHEC\n");
                exit(5);
            }
        }
        else if (pid>0) {
            wait(&k);
            if (WEXITSTATUS(k) != 5) {
                printf("SUCCES\n");
            }
        } 
        else {
            exit(1);
        }
    }
    printf("Salut\n");
}

        
