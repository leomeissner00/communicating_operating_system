#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "readcmd.h"

int main(int argc, char *argv[]) {

    struct cmdline *commande; /* ligne de commande entrée dans le minishell*/ 
    pid_t Fils ; /* pid pour reconnaitre le fils et le pere */
    int resultat;
    
    while(1){
        printf(">>>");
        commande = readcmd();
        
        if((commande -> seq[0] == NULL) || (strcmp(commande -> seq[0][0], "exit") == 0)) {
            break; /* le minishell s'arrête si on entre crtl D ou exit */
        }
        if(strcmp(commande -> seq[0][0], "cd") == 0) {
            chdir(commande-> seq[0][1]);
        } else {
            Fils = fork();
            if (Fils == -1) {
            
                perror("fork\n"); /* si erreur dans le fork */
                exit(EXIT_FAILURE);
            }
            if (Fils == 0) {
                execvp(commande->seq[0][0] ,commande->seq[0]); /* execution dans le fils */
                exit(EXIT_FAILURE);
            } else {
                if (commande->backgrounded == NULL){
                    waitpid(Fils, &resultat, WUNTRACED); /* attente et verification du resultat dans le pere */
                }
            }
        }
        
    }
}
