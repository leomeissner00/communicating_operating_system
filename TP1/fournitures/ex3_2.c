#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int fils = fork();
    if (fils < 0 ){
        perror("Erreur");
        exit(EXIT_FAILURE);
    } else if (fils == 0){
        char* commande = "/bin/ls";
        execvp(commande, &commande);
    } else {
        wait(&fils);
    }
    return EXIT_SUCCESS; 
}
