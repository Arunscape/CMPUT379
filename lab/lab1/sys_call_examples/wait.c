#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        printf("Child\n");
        sleep(2);
        return 1;
    }

    printf("Parent %d\n", pid);
    if (wait(NULL) ==  -1) {
        perror("wait");
        return 1;
    }

    printf("child dead\n");

    return 0;
}
