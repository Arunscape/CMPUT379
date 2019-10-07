#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
	    sleep(1);
    } else {
        // parent goes down this path (original process)
        int w_status;
        // on success wait returns the PID of the terminated child
        // on error, -1 is returned
        // int wc = wait(NULL);
        int wc = wait(&w_status);
        printf("hello, I am parent of %d (wc:%d, child terminated normally:%d, child terminated by a signal:%d) (pid:%d)\n", rc, wc, WIFEXITED(w_status), WIFSIGNALED(w_status), (int) getpid());
    }
    return 0;
}
