#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

// signal/signal1.c
void signal_callback_handler(int signum) {
	printf("Caught signal!\n");

	// uncomment this line to break the loop
	// when signal is received
	// exit(1);
}

int main() {
	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = signal_callback_handler;
	sigaction(SIGINT, &sa, NULL);
	// sigaction(SIGTSTP, &sa, NULL);
	while (1) {}
}