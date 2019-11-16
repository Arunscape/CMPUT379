#include <stdio.h>
#include <unistd.h>

#define MAXLINE 128

int main(void) {
	int n;
	int fd[2];
	pid_t pid;
	char line[MAXLINE];

	if (pipe(fd) < 0)			/* create a pipe before forking a child */
		perror("pipe error");
	if ((pid = fork()) < 0) {	/* fork a child */
		perror("fork error");
	} else if (pid > 0) {		/* parent continues */
		close(fd[0]);			/* close the unused end of the pipe */
		write(fd[1], "hello world!", 13);
	} else {					/* child continues */
		close(fd[1]);			/* close the unused end of the pipe */
		n = read(fd[0], line, MAXLINE);
		write(STDOUT_FILENO, line, n);
	}
	_exit(0);
}