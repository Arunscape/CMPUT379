#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    char *args[2] = {argv[1], NULL};
    char *envp[1] = {NULL};

    if (execve(argv[1], args, envp) == -1) {
        perror("execve");
        return 1;
    }

    printf("well hello there\n");
    return 0;
}
