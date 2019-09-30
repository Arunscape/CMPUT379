#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        sleep(1); // what happends if I uncomment out this?
        printf("Child\n");
    } else {
        sleep(1);
        printf("Parent\n");
    }

    return 0;
}
