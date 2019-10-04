#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <limits.h>
// #define _POSIX_C_SOURCE 1

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

#include "array.h"
#include "util.h"
#include "paths.h"

bool split_semicolon_and_run(char *buffer);
bool look_for_ampersand_and_run(char *buffer);
bool split_pipe_and_run(char *buffer, int sin, int sout, bool run_in_background);
bool split_redirect_and_run(char *buffer, int sin, int sout, int close_me);
bool split_space_and_run(char *buffer, int sin, int sout, int close_me);
bool do_commands();
void exec_from_path(struct array *tokens, int sin, int sout, char *path, int close_me);

bool run_line(char *buffer)
{
  return split_semicolon_and_run(buffer);
}

bool split_semicolon_and_run(char *buffer)
{
  bool ret = true;
  char *strtok_state = NULL;
  for (char *token = strtok_r(buffer, ";", &strtok_state); token != NULL; token = strtok_r(NULL, ";", &strtok_state))
  {
    ret = ret && look_for_ampersand_and_run(token);
  }
  return ret;
}

bool look_for_ampersand_and_run(char *buffer)
{
  char *ampersand = strstr(buffer, "&"); // assignment spec says & is always at the end of a command
  strtok(buffer, "&");                   //remove the &
  if (ampersand != NULL)
  {
    int fd = open("/dev/null", O_WRONLY);
    if (fd == -1)
    {
      perror("failed to open /dev/null");
    }
    return split_pipe_and_run(buffer, fd, fd, true);
  }
  return split_pipe_and_run(buffer, STDIN_FILENO, STDOUT_FILENO, false);
}

bool split_pipe_and_run(char *buffer, int sin, int sout, bool run_in_background)
{
  char *strtok_state = NULL;
  char *first_token = strtok_r(buffer, "|", &strtok_state);
  char *second_token = strtok_r(NULL, "|", &strtok_state);

  if (second_token != NULL)
  {
    int pipefd[2];
    if (pipe(pipefd) == 0)
    {
      split_redirect_and_run(first_token, sin, pipefd[1], pipefd[0]);
      split_redirect_and_run(second_token, pipefd[0], sout, pipefd[1]);
      close(pipefd[1]);
      close(pipefd[0]);
      if (!run_in_background)
      {
        wait(NULL);
        wait(NULL);
      }
    }
    else
    {
      perror("pipe failed");
    }
    // return false; // exit
    return true;
  }
  bool ret = split_redirect_and_run(first_token, sin, sout, -1);
  if (!run_in_background)
  {
    wait(NULL);
  }
  return ret;
}

bool split_redirect_and_run(char *buffer, int sin, int sout, int close_me)
{
  char *strtok_state = NULL;
  char *first_token = strtok_r(buffer, ">", &strtok_state);
  char *second_token = strtok_r(NULL, "> ", &strtok_state);

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
    }
    sout = fd;
  }
  return split_space_and_run(first_token, sin, sout, close_me);
}

bool split_space_and_run(char *buffer, int sin, int sout, int close_me)
{
  struct array tokens = create_array(sizeof(char *));
  tokenize(buffer, " ", &tokens);
  char *p = NULL;
  push_to_array(&tokens, &p);
  return do_commands(&tokens, sin, sout, close_me);
}

bool do_commands(struct array *tokens, int sin,
                 int sout, int close_me)
{
  if (strcmp(*((char **)get_from_array(tokens, 0)), "cd") == 0)
  {
    if (tokens->size > 3)
    {
      fprintf(stderr, "Too many args for cd command, expected 1\n");
    }
    else if (tokens->size < 3)
    {
      fprintf(stderr, "Expected one arg for cd\n");
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
    }
    print_path();
  }
  else if (strcmp(*((char **)get_from_array(tokens, 0)), "a2path") == 0)
  {
    if (tokens->size > 3)
    {
      fprintf(stderr, "Too many arguments, expected 1 arg for a2path\n");
    }
    else if (tokens->size < 3)
    {
      fprintf(stderr, "Expected 1 arg for a2path\n");
    }
    else if (strlen(*((char **)get_from_array(tokens, 1))) < 7 ||
             strncmp(*((char **)get_from_array(tokens, 1)), "$PATH:", 6) !=
                 0)
    {
      fprintf(stderr, "Incorrect syntax for 2nd arg in a2path. It should start "
                      "with $PATH: and then have a path to add.\nFor example: "
                      "a2path $PATH:/usr/local/bin\n");
    }
    add_to_path(*(char **)get_from_array(tokens, 1) + sizeof(char *) * 7);
  }
  else if ((strcmp(*((char **)get_from_array(tokens, 0)), "exit")) == 0)
  {
    if (tokens->size > 2)
    {
      fprintf(stderr, "Expected 0 args for exit\n");
    }
    free(tokens->array_ptr);
    return false; // exit
  }
  else
  { // look for executables
    char *path = get_absolute_path(*((char **)get_from_array(tokens, 0)));
    if (path == NULL)
    {
      fprintf(stderr, "No command found\n");
    }
    else
    {
      exec_from_path(tokens, sin, sout, path, close_me);
    }
  }
  free(tokens->array_ptr);
  return true;
}

void exec_from_path(struct array *tokens, int sin, int sout, char *path, int close_me)
{
  __pid_t f = fork();
  if (!f) // child
  {
    close(close_me);
    if (sin != STDIN_FILENO && dup2(sin, STDIN_FILENO) == -1)
    {
      perror("dup2");
      close(sin);
      close(sout);
      free(path);
      return;
    }

    if (sout != STDOUT_FILENO && dup2(sout, STDOUT_FILENO) == -1)
    {
      perror("dup2");
      close(sin);
      close(sout);
      free(path);
      return;
    }
    execve(path, tokens->array_ptr, NULL);
  }

  free(path);
  // free(tokens->array_ptr);
}