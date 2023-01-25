#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include "readcmd.h"
#define TRUE 1
#define NBMAXPROC 20

enum state {actif, suspendu};
typedef enum state state;

typedef struct proc {
    int id;
    pid_t pid;
    state etat;
    char *lcmd;
} proc ;

proc enCours[NBMAXPROC];
int indProc = 0;

void ajouterProc(proc process){
    enCours[indProc] = process;
    indProc++;  
}

void supprimerProc(int idSuppr){
    int i = 0;
    while(enCours[i].id != idSuppr && i <= indProc){
        i = i+1;
    }
    if(i > indProc){
        printf("Identifiant inexistant");
    } else if(i == indProc) {
        indProc--;
    } else {
        while(i < indProc){
            enCours[i] = enCours[i+1];
            i++;
        }
        indProc--;
    }
}

void supprimerProcPID(pid_t pidSuppr){
    int i = 0;
    while(enCours[i].pid != pidSuppr && i <= indProc){
        i = i+1;
    }
    if(i > indProc){
        printf("PID inexistant");
    } else if(i == indProc) {
        indProc--;
    } else {
        while(i < indProc){
            enCours[i] = enCours[i+1];
            i++;
        }
        indProc--;
    }
}

pid_t chercherPID(int identifiant){
    int i = 0;
    while(enCours[i].id != identifiant && i <= indProc){
        i = i+1;
    }
    if(i > indProc){
        printf("Identifiant inexistant");
    } else {
        return enCours[i].pid;
    }
    return -1;
}

void interrompreProc(int identifiant){
    int i = 0;
    while(enCours[i].id != identifiant && i <= indProc){
        i = i+1;
    }
    if(i > indProc){
        printf("Identifiant inexistant");
    } else {
        enCours[i].etat = suspendu;
        kill(chercherPID(identifiant), SIGSTOP);
    }
}

void reprendreProc(int identifiant, int pseudoBool){
    int i = 0;
    while(enCours[i].id != identifiant && i <= indProc){
        i = i+1;
    }
    if(i > indProc){
        printf("Identifiant inexistant");
    } else {
        enCours[i].etat = actif;
    }
    if (pseudoBool == 1) {
        int status;
        kill(chercherPID(identifiant), SIGCONT);
        waitpid(chercherPID(identifiant), &status, WUNTRACED);
        supprimerProc(identifiant);
    } else {
        kill(chercherPID(identifiant), SIGCONT);
    }
}

int idLibre(){
    int i=0;
    int j;
    int pseudoBool=0;
    while(i<=indProc){
       for(j=0; j<=indProc; j++){
           if(enCours[j].id==i){
               pseudoBool = 1;
               break;
           } else {
               pseudoBool = 0;
           }
        }
        if (pseudoBool == 1){
            i++;
        } else {
            return i;
        }
    }
    return 0;
}

void afficherProcessus(){
    int i;
    printf("Id : PID :  Etat :   Commande : \n");
    for (i = 0; i < indProc; i++){
        if (enCours[i].etat == actif){
            printf("%d    %d  %s %s \n", enCours[i].id, enCours[i].pid, "actif   ", enCours[i].lcmd);
        } else {
            printf("%d    %d  %s %s \n", enCours[i].id, enCours[i].pid, "suspendu", enCours[i].lcmd);
        }
    }
}

void background_handler(int sig, siginfo_t *info, void *ucontext) {
    supprimerProcPID(info->si_pid);
}

int main(int argc, char *argv[]) {

    //creation handler background
    struct sigaction sigac;
    sigac.sa_sigaction = &background_handler;
    sigac.sa_flags = SA_SIGINFO|SA_RESTART;
    sigemptyset(&sigac.sa_mask);
    
    sigaction(SIGCHLD, &sigac, NULL);
    
    //set de masquage
    //sigset_t masque;
    //sigaddset(&masque);
    //sigdelset(&masque, SIGCHLD);
    //sigprocmask(SIG_BLOCK, &masque, NULL);

    struct cmdline *cmd; /* contient la commande saisie au clavier */
    pid_t pidFils ; /* pid pour reconnaitre le fils et le pere */
    int result;
    while(TRUE){
        printf(">>>");
        cmd = readcmd(); /* structure de commande */
        if(cmd == NULL || (cmd->seq[0] == NULL) || strcmp(cmd->seq[0][0], "exit") == 0){
            break; /* on sort si exit est entre ou ctrl + D utilise*/}
        if(strcmp(cmd->seq[0][0], "cd") == 0){ 
            chdir(cmd->seq[0][1]);
        } else if (strcmp(cmd->seq[0][0], "lj") == 0){
            afficherProcessus();
        } else if (strcmp(cmd->seq[0][0], "sj") == 0){
            int idProc = strtol(cmd->seq[0][1], NULL, 10);
            interrompreProc(idProc);
        } else if (strcmp(cmd->seq[0][0], "bj") == 0){
            int idProc = strtol(cmd->seq[0][1], NULL, 10);
            reprendreProc(idProc,0);
        } else if (strcmp(cmd->seq[0][0], "fj") == 0){
            int idProc = strtol(cmd->seq[0][1], NULL, 10);
            reprendreProc(idProc,1);
        } else {
            pidFils = fork();
            if (pidFils == -1) {
                perror("fork\n"); /* si erreur dans le fork */
                exit(1);
            }
            if (pidFils == 0){
                execvp(cmd->seq[0][0],cmd->seq[0]); /* execution dans le fils */ 
                perror("Erreur execution");
                exit(1);
            } else {
                /* ajout du processus aux processus en cours*/
                proc process;
                process.id = idLibre();
                process.pid = pidFils;
                process.etat = actif;
                process.lcmd = malloc(8*strlen(cmd->seq[0][0]));
                strcpy(process.lcmd, cmd->seq[0][0]);
                ajouterProc(process);
                /* attente du fils si premier plan*/
                if(cmd->backgrounded == NULL) {
                    waitpid(pidFils,&result,WUNTRACED);
                } 
            }
        }
    }
}
