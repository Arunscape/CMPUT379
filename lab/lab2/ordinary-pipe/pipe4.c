#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) { 
	int fd[2];
	pid_t pid;
	
	if (pipe(fd) < 0) 
		perror("pipe error!"); 
	if ((pid = fork()) < 0) 
		perror("fork error!");
	if (pid == 0) {
		close(fd[1]); 				// child won’t write
		dup2(fd[0], STDIN_FILENO);	// stdin = fd[0]
		close(fd[0]); 				// stdin is still open 
		if (execl("/usr/bin/wc", "wc", "-w", (char *) 0) < 0)
			perror("execl error!"); 
	} else {
		close(fd[0]); 				// parent won’t read
		dup2(fd[1], STDOUT_FILENO);	// stdout = fd[1]
		close(fd[1]); 				// stdout is still open
		if (execl("/usr/bin/w", "w", (char *) 0) < 0)
			perror("execl error!");
	}
	return 0;
}