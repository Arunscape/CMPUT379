#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "array.h"

void attempt_evaluate_from_path();

void do_commands(struct array *tokens)
{

  if (strcmp(*((char **)get_from_array(tokens, 0)), "cd") == 0)
  {
    if (tokens->size > 2)
    {
      fprintf(stderr, "Too many args for cd command\n");
      return;
    }
    else if (tokens->size < 1)
    {
      fprintf(stderr, "Expected one arg for cd\n");
      return;
    }

    if (chdir(*((char **)get_from_array(tokens, 1))) == -1)
    {
      perror("chdir");
    }
  }
  else if (strcmp(*((char **)get_from_array(tokens, 0)), "pwd") == 0)
  {
    if (tokens->size > 1)
    {
      fprintf(stderr, "Expected 0 args for pwd");
      return;
    }
    char cwd[PATH_MAX];
    if (getcwd(cwd, PATH_MAX) != NULL)
    {
      printf("%s\n", cwd);
    }
    else
    {
      perror("getcwd() error");
    }
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