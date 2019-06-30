/* execute.c - code used by small shell to execute commands */

#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/wait.h>

int execute(char *list[]) {
    int pid, exitstatus;
    // printf("the first arg is %s\n", list[0]);
    
    if (list[0] == NULL) {
        return 0;
    }
    
    
    switch (pid = fork()) {
        case -1:
            perror("fork() failed");
        case 0:
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            execvp(list[0], list);

            perror("Error execvp");
            exit(1);
    
        default:
            wait(&exitstatus);
            printf("Process %d finished with status %d\n", pid, exitstatus);
            break;
    }
    return exitstatus;
}