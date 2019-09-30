#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    pid_t pid = atoi(argv[1]);

    if (kill(pid, SIGINT) == -1) {
        perror("kill");
        return 1;
    }

    return 0;
}
