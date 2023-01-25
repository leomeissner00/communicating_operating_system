#include <stdio.h> /* printf */
#include <unistd.h> /* fork */
#include <stdlib.h> /* EXIT_SUCCESS */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        perror("Erreur nombre argument");
    }

    int destination = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
    
    if (destination == -1) {
        perror("erreur open");
        exit(1);
    }
      
    dup2(destination, 1);
    close(destination);
    
    execlp("ls", "ls", "-u", NULL);
    
    perror("Erreur execution");
    exit(1);
    
    return EXIT_SUCCESS ;
}
