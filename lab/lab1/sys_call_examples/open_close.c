#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define BUF_SIZE    20

int main() {
    // open
    int fd = open("open_close_test_file", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    printf("opened %d\n", fd);

    // write to file
    char str[] = "Hello World\n";
    int str_len = sizeof(str);
    int wrtn = write(fd, str, str_len);
    if (wrtn == -1) {
        perror("write");
        close(fd);
        return 1;
    } else if (wrtn != str_len) {
        fprintf(stderr, "only %d bytes written\n", wrtn);
    }

    // close, what happens if I comment this out?
    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    // open
    fd = open("open_close_test_file", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    printf("opened %d\n", fd);

    // read
    size_t size = BUF_SIZE;
    char buf[BUF_SIZE] = {0};

    if (read(fd, buf, size) == -1) {
        perror("read");
        return 1;
    }

    printf("%s", buf);

    // close
    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    return 0;
}
