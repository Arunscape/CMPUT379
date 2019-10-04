#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "array.h"
#include "util.h"
#include "paths.h"

void split_semicolon_and_run(char *buffer);
void split_pipe_and_run(char *buffer);
void split_redirect_and_run(char *buffer, int sin, int sout);
void split_space_and_run(char *buffer, int sin, int sout);
void do_commands();
void exec_from_path(struct array *tokens, int sin, int sout, char *path);

void run_line(char *buffer)
{
  split_semicolon_and_run(buffer);
}

void split_semicolon_and_run(char *buffer)
{
  char *strtok_state = NULL;
  for (char *token = strtok_r(buffer, ";", &strtok_state); token != NULL; token = strtok_r(NULL, ";", &strtok_state))
  {
    split_pipe_and_run(token);
  }
}

void split_pipe_and_run(char *buffer)
{
  char *strtok_state = NULL;
  char *first_token = strtok_r(buffer, "|", &strtok_state);
  char *second_token = strtok_r(NULL, "|", &strtok_state);

  if (second_token != NULL)
  {
    int pipefd[2];
    if (pipe(pipefd) == 0)
    {
      printf("pipefd: %d, SOUT: %d\n", pipefd[0], pipefd[1]);
      fprintf(stderr, "BLIP\n");
      split_redirect_and_run(first_token, STDIN_FILENO, pipefd[1]);
      fprintf(stderr, "BLIP\n");
      split_redirect_and_run(second_token, pipefd[0], STDOUT_FILENO);
      fprintf(stderr, "BLIP\n");
      wait(NULL);
      fprintf(stderr, "BLIP\n");
      close(pipefd[0]);
      close(pipefd[1]);
      fprintf(stderr, "BLIP\n");
      wait(NULL);
      fprintf(stderr, "BLIP\n");
    }
    else
    {
      perror("pipe failed");
    }
    return;
  }
  split_redirect_and_run(first_token, STDIN_FILENO, STDOUT_FILENO);
  wait(NULL);
}

void split_redirect_and_run(char *buffer, int sin, int sout)
{
  char *strtok_state = NULL;
  char *first_token = strtok_r(buffer, ">", &strtok_state);
  char *second_token = strtok_r(NULL, ">", &strtok_state);

  if (second_token != NULL)
  {
    if (sout != STDOUT_FILENO)
    { //close sout if we're not redirecting
      close(sout);
    }
    // WE ARE ASSUMING THAT THIS IS A FILE
    int fd = open(second_token, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1)
    {
      perror("failed to open file");
      return;
    }
    sout = fd;
  }
  split_space_and_run(first_token, sin, sout);
}

void split_space_and_run(char *buffer, int sin, int sout)
{
  struct array tokens = create_array(sizeof(char *));
  tokenize(buffer, " ", &tokens);
  char *p = NULL;
  push_to_array(&tokens, &p);
  do_commands(&tokens, sin, sout);
}

void do_commands(struct array *tokens, int sin,
                 int sout)
{
  if (strcmp(*((char **)get_from_array(tokens, 0)), "cd") == 0)
  {
    if (tokens->size > 3)
    {
      fprintf(stderr, "Too many args for cd command, expected 1\n");
      return;
    }
    else if (tokens->size < 3)
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
    if (tokens->size > 2)
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
    if (tokens->size > 2)
    {
      fprintf(stderr, "Expected 0 args for $PATH\n");
      return;
    }
    print_path();
  }
  else if (strcmp(*((char **)get_from_array(tokens, 0)), "a2path") == 0)
  {
    if (tokens->size > 3)
    {
      fprintf(stderr, "Too many arguments, expected 1 arg for a2path\n");
      return;
    }
    else if (tokens->size < 3)
    {
      fprintf(stderr, "Expected 1 arg for a2path\n");
      return;
    }
    else if (strlen(*((char **)get_from_array(tokens, 1))) < 7 ||
             strncmp(*((char **)get_from_array(tokens, 1)), "$PATH:", 6) !=
                 0)
    {
      fprintf(stderr, "Incorrect syntax for 2nd arg in a2path. It should start "
                      "with $PATH: and then have a path to add.\nFor example: "
                      "a2path $PATH:/usr/local/bin\n");
      return;
    }
    add_to_path(*(char **)get_from_array(tokens, 1) + sizeof(char *) * 7);
  }
  else if ((strcmp(*((char **)get_from_array(tokens, 0)), "exit")) == 0)
  {
    if (tokens->size > 2)
    {
      fprintf(stderr, "Expected 0 args for exit\n");
      return;
    }
    // GLOBALS->EXIT = true; // TODO IMPLEMENT EXIT
  }
  else
  {
    // attempt_evaluate_from_path(tokens, GLOBALS, STDIN_FILENO, STDOUT_FILENO);
    char *path = get_absolute_path(*((char **)get_from_array(tokens, 0)));
    if (path == NULL)
    {
      fprintf(stderr, "No command found\n");
    }
    else
    {
      exec_from_path(tokens, sin, sout, path);
    }
  }
}

void exec_from_path(struct array *tokens, int sin, int sout, char *path)
{
  printf("SIN: %d, SOUT: %d\n", sin, sout);
  __pid_t f = fork();
  if (!f) // child
  {
    if (sin != STDIN_FILENO && dup2(sin, STDIN_FILENO) == -1)
    {
      perror("dup2");
      close(sin);
      return;
    }

    if (sout != STDOUT_FILENO && dup2(sout, STDOUT_FILENO) == -1)
    {
      perror("dup2");
      close(sout);
      close(sin);
      return;
    }
    fprintf(stderr, "HEEEEY%d\n", sout);
    execve(path, tokens->array_ptr, NULL);
  }

  free(path);
}