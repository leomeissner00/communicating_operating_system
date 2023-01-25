#define NBMAXPROC 20
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include "readcmd.h"

enum activite {actif, suspendu};
typedef enum activite activite;

typedef struct proc {
    int id;
    pid_t pid;
    activite etat;
    char *lacommande;
} proc ;

proc tabProc[NBMAXPROC];
int indEnCours = 0;

void listerProc(proc processus){
    tabProc[indEnCours] = processus;
    indEnCours++;  
}

int idLibre(){
    int compteur=0;
    int k;
    int checkBjFj=0;
    while(compteur<=indEnCours){
       for(k=0; k<=indEnCours; k++){
           if(tabProc[k].id==compteur){
               checkBjFj = 1;
               break;
           } else {
               checkBjFj = 0;
           }
        }
        if (checkBjFj == 1){
            compteur++;
        } else {
            return compteur;
        }
    }
    return 0;
}

void afficherProc(){
    int compteur;
    printf("Id : PID :  Etat :   Commande : \n");
    for (compteur = 0; compteur < indEnCours; compteur++){
        if (tabProc[compteur].etat == actif){
            printf("%d    %d  %s %s \n", tabProc[compteur].id, tabProc[compteur].pid, "actif   ", tabProc[compteur].lacommande);
        } else {
            printf("%d    %d  %s %s \n", tabProc[compteur].id, tabProc[compteur].pid, "suspendu", tabProc[compteur].lacommande);
        }
    }
}

void enleverProc(int idSuppr){
    int compteur = 0;
    while(tabProc[compteur].id != idSuppr && compteur <= indEnCours){
        compteur = compteur+1;
    }
    if(compteur > indEnCours){
        printf("ID inexistant");
    } else if(compteur == indEnCours) {
        indEnCours--;
    } else {
        while(compteur < indEnCours){
            tabProc[compteur] = tabProc[compteur+1];
            compteur++;
        }
        indEnCours--;
    }
}

pid_t checkerPID(int ID){
    int compteur = 0;
    while(tabProc[compteur].id != ID && compteur <= indEnCours){
        compteur = compteur+1;
    }
    if(compteur <= indEnCours){
        return tabProc[compteur].pid;
    } else {
        printf("ID inexistant");
    }
    return -1;
}

void enleverProcPID(pid_t pidSuppr){
    int compteur = 0;
    while(tabProc[compteur].pid != pidSuppr && compteur <= indEnCours){
        compteur = compteur+1;
    }
    if(compteur > indEnCours){
        printf("PID inexistant");
    } else if(compteur == indEnCours) {
        indEnCours--;
    } else {
        while(compteur < indEnCours){
            tabProc[compteur] = tabProc[compteur+1];
            compteur++;
        }
        indEnCours--;
    }
}

void arreterProc(int ID){
    int compteur = 0;
    while(tabProc[compteur].id != ID && compteur <= indEnCours){
        compteur = compteur+1;
    }
    if(compteur <= indEnCours){
        tabProc[compteur].etat = suspendu;
        kill(checkerPID(ID), SIGSTOP);
    } else {
        printf("ID inexistant");
    }
}

void rallumerProc(int ID, int checkBjFj){
    int compteur = 0;
    while(tabProc[compteur].id != ID && compteur <= indEnCours){
        compteur = compteur+1;
    }
    if(compteur <= indEnCours){
        tabProc[compteur].etat = actif;
    } else {
        printf("ID inexistant");
    }
    if (checkBjFj == 1) {
        int status;
        kill(checkerPID(ID), SIGCONT);
        waitpid(checkerPID(ID), &status, WUNTRACED);
        enleverProc(ID);
    } else {
        kill(checkerPID(ID), SIGCONT);
    }
}

void background_handler(int sig, siginfo_t *info, void *ucontext) {
    enleverProcPID(info->si_pid);
}

int main(int argc, char *argv[]) {

    //creation handler background
    struct sigaction monsig;
    monsig.sa_sigaction = &background_handler;
    monsig.sa_flags = SA_SIGINFO|SA_RESTART;
    sigemptyset(&monsig.sa_mask);
    
    sigaction(SIGCHLD, &monsig, NULL);

    struct cmdline *commande; /*structure de commande saisie par l'utilisateur */
    pid_t Fils ; /* pid du fils */
    int resultat;
    while(1){
        printf(">>>");
        commande = readcmd(); /* commande entrée par l'utilisateur */
        if(commande == NULL || (commande->seq[0] == NULL) || strcmp(commande->seq[0][0], "exit") == 0){
            break; /* arret lorsque  exit est entre ou ctrl + D*/}
        if(strcmp(commande->seq[0][0], "cd") == 0){ 
            chdir(commande->seq[0][1]);
        } else if (strcmp(commande->seq[0][0], "lj") == 0){
            afficherProc();
        } else if (strcmp(commande->seq[0][0], "sj") == 0){
            int idProc = strtol(commande->seq[0][1], NULL, 10);
            arreterProc(idProc);
        } else if (strcmp(commande->seq[0][0], "bj") == 0){
            int idProc = strtol(commande->seq[0][1], NULL, 10);
            rallumerProc(idProc,0);
        } else if (strcmp(commande->seq[0][0], "fj") == 0){
            int idProc = strtol(commande->seq[0][1], NULL, 10);
            rallumerProc(idProc,1);
        } else {
            Fils = fork();
            if (Fils == -1) {
                perror("fork\n"); /* lorsque erreur avec le fork */
                exit(1);
            }
            if (Fils == 0){
                /* execution du fils */ 
                execvp(commande->seq[0][0],commande->seq[0]); 
                perror("Erreur execution");
                exit(1);
            } else {
                /* ajout du processus aux processus en cours*/
                proc processus;
                processus.lacommande = malloc(8*strlen(commande->seq[0][0]));
                processus.id = idLibre();
                processus.etat = actif;
                processus.pid = Fils;
                
                strcpy(processus.lacommande, commande->seq[0][0]);
                listerProc(processus);

                /* attente du fils lorsqu'il est en premier plan*/
                if(commande->backgrounded == NULL) {
                    waitpid(Fils,&resultat,WUNTRACED);
                } 
            }
        }
    }
}
