#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

int print_path() {
    char path[PATH_MAX];

    // print path
    if (getcwd(path, PATH_MAX) == NULL) {
        perror("getcwd");
        return 0;
    }

    printf("path: %s\n", path);

    return 1;
}

int main(int argc, char *argv[]) {
    if (!print_path()) {
        return 1;
    }

    // chdir
    if (chdir(argv[1]) == -1) {
        perror("chdir");
        return 1;
    }

    if (!print_path()) {
        return 1;
    }

    return 0;
}
