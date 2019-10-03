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
#include "globals.h"
#include "util.h"

void do_commands();
void attempt_evaluate_from_path();

void determine_what_to_do(struct globals *GLOBALS, char *buffer)
{

  // begin semicolon handling
  char *buffer_copy = strdup(buffer);
  char *semicolon_token_strtok_state = NULL;
  char *semicolon_token = strtok_r(buffer_copy, ";", &semicolon_token_strtok_state);
  char *first_semicolon_token = strdup(semicolon_token);
  // end semicolon handling

  // begin redirection handling
  char *buffer_copy2 = strdup(first_semicolon_token);
  char *redirect_token_strtok_state = NULL;
  char *redirect_token = strtok_r(buffer_copy2, ">", &redirect_token_strtok_state);
  char *first_redirect_token = strdup(redirect_token);
  char *buffer_copy3 = strdup(first_redirect_token); // actually, this is for pipe
  if ((redirect_token = strtok_r(NULL, ">", &redirect_token_strtok_state)) != NULL)
  {
    int fd = open(redirect_token, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1)
    {
      perror("open");
      return;
    }

    struct array tokens = create_array(sizeof(char *));
    tokenize(first_semicolon_token, " ", &tokens); // &mut tokens
    do_commands(&tokens, GLOBALS, stdin, fd, stderr);
    free(tokens.array_ptr);
    return;
  }
  // end redirection handling

  // begin pipe handling
  char *pipe_token_strtok_state = NULL;
  char *pipe_token = strtok_r(buffer_copy3, "|", &pipe_token_strtok_state);
  char *first_pipe_token = strdup(pipe_token);
  // end pipe handling

  struct array tokens = create_array(sizeof(char *));
  tokenize(first_semicolon_token, " ", &tokens); // &mut tokens

  // normal stdin and stdout
  do_commands(&tokens, GLOBALS, stdin, STDOUT_FILENO, stderr);

  while ((semicolon_token = strtok_r(NULL, ";", &semicolon_token_strtok_state)) != NULL)
  {
    printf("Here's what got separated: %s\n", (char *)semicolon_token);
    determine_what_to_do(GLOBALS, semicolon_token);
  }
  free(buffer_copy);
  free(buffer_copy2);
  free(buffer_copy3);

  free(first_semicolon_token);
  free(semicolon_token);

  free(first_redirect_token);
  free(redirect_token);

  free(first_pipe_token);
  // free(pipe_token);

  free(tokens.array_ptr);
}

void do_commands(struct array *tokens, struct globals *GLOBALS, FILE *sin,
                 int sout, FILE *serr)
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
    else if (strlen(*((char **)get_from_array(tokens, 1))) < 7 ||
             strncmp(*((char **)get_from_array(tokens, 1)), "$PATH:", 6) !=
                 0)
    {
      fprintf(stderr, "Incorrect syntax for 2nd arg in a2path. It should start "
                      "with $PATH: and then have a path to add.\nFor example: "
                      "a2path $PATH:/usr/local/bin\n");
      return;
    }
    // in the future, use strtok and asprintf LOL
    char *new_path = malloc(
        strlen(GLOBALS->PATH) +
        strlen(strchr(*((char **)get_from_array(tokens, 1)), (int)":") - 1) +
        1);
    strcpy(new_path, GLOBALS->PATH);
    strcat(new_path,
           strchr(*((char **)get_from_array(tokens, 1)), (int)":") - 1);
    free(GLOBALS->PATH);
    GLOBALS->PATH = new_path;
  }
  else if ((strcmp(*((char **)get_from_array(tokens, 0)), "exit")) == 0)
  {
    if (tokens->size > 1)
    {
      fprintf(stderr, "Expected 0 args for exit\n");
      return;
    }
    GLOBALS->EXIT = true;
  }
  else
  {
    attempt_evaluate_from_path(tokens, GLOBALS, sin, sout, serr);
  }
}

void attempt_evaluate_from_path(struct array *tokens, struct globals *GLOBALS, FILE *sin, int sout, FILE *serr)
{

  struct array paths = create_array(sizeof(char *));
  char *path_copy = strdup(GLOBALS->PATH);

  tokenize(path_copy, ":", &paths); // &mut paths

  // next time, loop over strtok
  for (int i = 0; i < paths.size; i += 1)
  {
    char *concat = NULL;
    asprintf(&concat, "%s/%s", *(char **)get_from_array(&paths, i),
             *(char **)get_from_array(tokens, 0));

    *(char **)get_from_array(&paths, i) = concat;
  }
  char *cwd = getcwd(NULL, PATH_MAX);
  char *concat = NULL;
  asprintf(&concat, "%s/%s", cwd, *(char **)get_from_array(tokens, 0));
  free(cwd);
  push_to_array(&paths, &concat);

  __pid_t f = fork();
  if (!f) // child
  {
    if (dup2(sout, STDOUT_FILENO) == -1)
    {
      perror("dup2");
      close(sout);
      return;
    }

    char *p = NULL;
    push_to_array(tokens, &p);
    if (access(*(char **)get_from_array(tokens, 0), F_OK | X_OK) ==
        0)
    { // attempt to execute the absolute path
      execve(*(char **)get_from_array(tokens, 0), tokens->array_ptr, NULL);
    }
    for (int i = 0; i < paths.size; i += 1)
    {

      if (access(*(char **)get_from_array(&paths, i), F_OK | X_OK) == 0)
      {
        execve(*(char **)get_from_array(&paths, i), tokens->array_ptr, NULL);
      }

      else
      {
        ;
      }
    }
  }
  wait(NULL); // wait for child process to terminate

  // cleanup
  for (int i = 0; i < paths.size; i += 1)
  {
    free(*(void **)get_from_array(&paths, i));
  }
  free(paths.array_ptr);
  free(path_copy);
  // free(concat);
}