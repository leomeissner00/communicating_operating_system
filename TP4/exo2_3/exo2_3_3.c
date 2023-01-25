#include <stdio.h> /* printf */
#include <unistd.h> /* fork */
#include <stdlib.h> /* EXIT_SUCCESS */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/wait.h>

int main() {
    int n;
    int i;
    int e;
    int p[2];
    pipe(p);
    int pid_Fils = fork();
    if (pid_Fils == -1) {
        perror("fork\n"); /* lorsque erreur avec le fork */
        exit(1);
    }
    if (pid_Fils == 0){
        close(p[1]);
        i = 100;
        while((i = read(p[0], &e, sizeof(e))) != 0){
            printf("%d\n", e);
        }
        
    } else {
        close(p[0]);
        n = 10;
        for (i = 1; i <= n ; i++) {
            write(p[1], &i, sizeof(i));
        }
        //waitpid(pid_Fils, &n, WUNTRACED);

        close(p[1]);
        sleep(10);
    }

    return EXIT_SUCCESS ;
}