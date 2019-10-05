#define _GNU_SOURCE

#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "array.h"

// I KNOW THIS IS BAD PRACTICE BUT THERE IS NO TIME
extern pid_t CHILD_PIDS[];
extern int CHILD_PIDS_SIZE;

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
  // printf("\n🐲 dragonshell🐉 > ");
}

void handle_signals()
{
  struct sigaction sa;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = signal_callback_handler;
  // sa.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTSTP, &sa, NULL);
}

void kill_children()
{
  for (int i = 0; i <= CHILD_PIDS_SIZE; i += 1)
  {
    if (CHILD_PIDS[i] == 0)
    {
      continue;
    }
    kill(CHILD_PIDS[i], SIGTERM); // politely tell it to end itself
    waitpid(CHILD_PIDS[i], NULL, 0);
    kill(CHILD_PIDS[i], SIGKILL); // FORCE IT
  }
}