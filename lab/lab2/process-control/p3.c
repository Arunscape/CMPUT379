#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
        char *myargs[4];
        char *env[] = {NULL};
        myargs[0] = strdup("/usr/bin/wc");   // program: "wc" (word count)
        myargs[1] = strdup("-w");   // argument: print the word counts
        myargs[2] = strdup("p3.c"); // argument: file to count
        myargs[3] = NULL;           // marks end of array
        if (execve(myargs[0], myargs, env) < 0)  // runs word count
            perror("execve");
        printf("this shouldn't print out\n");
    } else {
        // parent goes down this path (original process)
        int wc = wait(NULL);
        printf("hello, I am parent of %d (wc:%d) (pid:%d)\n",
	       rc, wc, (int) getpid());
    }
    return 0;
}
