#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int  i;

void quit(int signum) {
	fprintf(stderr, "\nInterrupt (code= %d, i= %d)\n", signum, i);
}

int main () {
	if(signal(SIGQUIT, quit) == SIG_ERR)
		perror("can′t catch SIGQUIT");
	for (i= 0; 1; i++) {
		usleep(1000);
		if (i % 100 == 0) putc('.', stderr);
	}
	return(0);
 } // signal/signal2.c