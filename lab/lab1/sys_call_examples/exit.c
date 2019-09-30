#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
    int ret = 10;

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // sleep(1); // what happends if I uncomment out this?
        printf("Child\n");
        _exit(ret);
    }

    printf("Parent %d\n", pid);
    pid = wait(&ret);
    if (pid == -1) {
        perror("wait");
        return 1;
    }

    printf("ret %d\npid %d\n", WEXITSTATUS(ret), pid);

    return 0;
}
