#define _GNU_SOURCE

#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "array.h"

// tokenize function modified to work with the custom c array
// we're storing pointers to the char** strings in the array
void tokenize(char *str, const char *delim, struct array *array)
{
  char *token_state = NULL;
  char *token = strtok_r(str, delim, &token_state);
  for (size_t i = 0; token != NULL; i += 1)
  {
    push_to_array(array, &token);
    token = strtok_r(NULL, delim, &token_state);
  }
}

void signal_callback_handler(int signum)
{

  fprintf(stdout, "\n");
}

void handle_signals()
{
  struct sigaction sa;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = signal_callback_handler;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTSTP, &sa, NULL);
}

void kill_children(struct array *CHILD_PIDS)
{
  printf("KILLING CHILDREN\n");
  printf("size: %d\n", CHILD_PIDS->size);
  for (int i = 0; i < CHILD_PIDS->size; i += 1)
  {
    pid_t pid = *(pid_t **)get_from_array(CHILD_PIDS, i);
    printf("%ld\n", (long)pid);
    kill(pid, SIGKILL); // FORCE IT
    waitpid(pid, NULL, 0);
    // kill(pid, SIGTERM); // politely tell it to end itself
  }
}