#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

int main (int argc, char *argv[]) {
    int wcgrep[2];
    int grepwho[2];
    pipe(wcgrep);

    int pidpere = fork();
    if (pidpere == -1) {
        perror("erreur fork");
        exit(1);
    } else if (pidpere == 0) {
        close(wcgrep[0]);

        pipe(grepwho);

        int pid_fils = fork();
        if (pid_fils == -1) {
            perror("erreur fork");
            exit(1);
        } else if (pid_fils == 0) {
            close(wcgrep[1]);
            close(grepwho[0]);
            dup2(grepwho[1],1);
            close(grepwho[0]);
            execlp("who", "who", NULL);
        } else {
            close(grepwho[1]);
            dup2(wcgrep[1], 1); 
            dup2(grepwho[0], 0); 
            close(wcgrep[1]);
            close(grepwho[0]);
            execlp("grep", "grep", argv[1], NULL);
        }
    } else {
        close(wcgrep[1]);
        dup2(wcgrep[0], 0);
        close(wcgrep[0]);
        execlp("wc", "wc", "-l", NULL);

    }
}