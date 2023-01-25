#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main (int argc, char *argv[]){
    int dest;
    dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666); /* 0444 pour lecture seule */
    dup2(dest, 1);
    close(dest);
    execlp("cat", "cat", argv[1], NULL);
}