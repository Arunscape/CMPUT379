#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
	int shared_int=0;
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d) and the shared data is %d\n", (int) getpid(), shared_int);
	printf("hello, I am child (pid:%d) and the modified shared data is %d\n", (int) getpid(), ++shared_int);
    } else {
        // parent goes down this path (original process)
        printf("hello, I am parent of %d (pid:%d) and the shared data is %d\n", rc, (int) getpid(), shared_int);
	printf("hello, I am parent of %d (pid:%d) and the modified shared data is %d\n", rc, (int) getpid(), --shared_int);
    }
    return 0;
}
