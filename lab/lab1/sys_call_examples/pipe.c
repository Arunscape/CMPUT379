#include <unistd.h>
#include <stdio.h>

#define BUF_SIZE    20

int main() {
    int p[2];

    if (pipe(p) == -1) {
        perror("pipe");
        return 1;
    }

    // write
    char str[] = "Hello\n";
    int str_len = sizeof(str);
    int wrtn = write(p[1], str, str_len);
    if (wrtn == -1) {
        perror("write");
        return 1;
    } else if (wrtn != str_len) {
        fprintf(stderr, "only %d bytes written\n", wrtn);
    }

    // read
    size_t size = BUF_SIZE;
    char buf[BUF_SIZE] = {0};

    if (read(p[0], buf, size) == -1) {
        perror("read");
        return 1;
    }

    printf("%s", buf);

    return 0;
}
