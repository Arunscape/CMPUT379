#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "array.h"
#include "globals.h"

void attempt_evaluate_from_path();

void do_commands(struct array *tokens, struct globals *GLOBALS)
{

  if (strcmp(*((char **)get_from_array(tokens, 0)), "cd") == 0)
  {
    if (tokens->size > 2)
    {
      fprintf(stderr, "Too many args for cd command, expected 1\n");
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
      fprintf(stderr, "Expected 0 args for pwd\n");
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
  else if (strcmp(*((char **)get_from_array(tokens, 0)), "$PATH") == 0)
  {
    if (tokens->size > 1)
    {
      fprintf(stderr, "Expected 0 args for $PATH\n");
      return;
    }
    fprintf(stdout, "Current PATH: %s\n", GLOBALS->PATH);
  }
  else if (strcmp(*((char **)get_from_array(tokens, 0)), "a2path") == 0)
  {
    if (tokens->size > 2)
    {
      fprintf(stderr, "Too many arguments, expected 1 arg for a2path\n");
      return;
    }
    else if (tokens->size < 2)
    {
      fprintf(stderr, "Expected 1 arg for a2path\n");
      return;
    }
    else if (strlen(*((char **)get_from_array(tokens, 1))) < 7 || strncmp(*((char **)get_from_array(tokens, 1)), "$PATH:", 6) != 0)
    {
      fprintf(stderr, "Incorrect syntax for 2nd arg in a2path. It should start with $PATH: and then have a path to add.\nFor example: a2path $PATH:/usr/local/bin\n");
      return;
    }
    char *new_path = malloc(strlen(GLOBALS->PATH) + strlen(strchr(*((char **)get_from_array(tokens, 1)), (int)":") - 1) + 1);

    strcat(new_path, GLOBALS->PATH);
    strcat(new_path, strchr(*((char **)get_from_array(tokens, 1)), (int)":") - 1);
    fprintf(stdout, "%s\n", new_path);
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