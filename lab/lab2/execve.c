#include <stdio.h>
#include <unistd.h>

int main() {
    int pid;
    char *argv1[]={"ls", "-al", NULL};
    char *env[] = {NULL};
    if ((pid = fork()) ==-1)
        perror("fork error");
    else if (pid == 0)
        if(execve("/bin/ls", argv1, env) == -1)
            perror("execve");
    return 0;
}

