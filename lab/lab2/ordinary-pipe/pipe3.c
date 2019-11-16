#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXBUF 128

int main (int argc, char *argv[]) { 
	char buf[MAXBUF];
	int n, status, fd[2];
	pid_t pid;
	
	if (pipe(fd) < 0) 
		perror("pipe error!"); 
	if ((pid = fork()) < 0) 
		perror("fork error!");
	if (pid == 0) {
		close(fd[0]); 				// child won’t read
		dup2(fd[1], STDOUT_FILENO);	// stdout = fd[1]
		close(fd[1]); 				// stdout is still open 
		if (execl("/usr/bin/w", "w", (char *) 0) < 0)
			perror("execl error!"); 
	} else {
		close(fd[1]); 				// parent won’t write 
		while ((n= read(fd[0], buf, MAXBUF)) > 0)
			write(STDOUT_FILENO, buf, n);
		close(fd[0]);
		wait(&status);
	}
	return 0;
}