#include <stdio.h> /* printf */
#include <unistd.h> /* fork */
#include <stdlib.h> /* EXIT_SUCCESS */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/wait.h>

int main() {
    int n;
    int p[2];
    int pid_Fils = fork();
    pipe(p);
    if (pid_Fils == -1) {
        perror("fork\n"); /* lorsque erreur avec le fork */
        exit(1);
    }
    if (pid_Fils == 0){
        close(p[1]);
        read(p[0], &n, sizeof(n));
        printf("%d\n", n);
    } else {
        close(p[0]);
        n = 3;
        write(p[1], &n, sizeof(n));
        //waitpid(pid_Fils, &n, WUNTRACED);
    }

    return EXIT_SUCCESS ;
}