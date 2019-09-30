#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "array.h"
void attempt_evaluate_from_path();

void do_commands(struct array *tokens)
{

  if (strcmp(*((char **)get_from_array(tokens, 0)), "cd") == 0)
  {
    char cwd[64];
    printf("current directory: %s\n", getcwd(cwd, sizeof(cwd)));
    fork();
    chdir(*((char **)get_from_array(tokens, 1)));
    printf("current directory: %s\n", getcwd(cwd, sizeof(cwd)));
  }
  else if (strcmp(*((char **)get_from_array(tokens, 0)), "pwd") == 0)
  {
    // char cwd[64];
    // if (getcwd(cwd, sizeof(cwd)) != NULL)
    // {
    //   printf("Current working dir: %s\n", cwd);
    // }
    // else
    // {
    //   perror("getcwd() error");
    // }
  }

  else
  {
    attempt_evaluate_from_path();
  }
}

void attempt_evaluate_from_path()
{
  __pid_t f = fork();
  char *argv[] = {"ls", "-al", NULL};
  char *envp[] = {"some", "environment", NULL};
  if (f)
  {
    // wait();
    // execve("/bin/ls", argv, envp);
  }
  else
  {
    execve("/bin/ls", argv, envp);
  }
}