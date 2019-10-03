#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "array.h"
#include "globals.h"
#include "util.h"

void determine_what_to_do();
void do_commands();
void attempt_evaluate_from_path();

void determine_what_to_do(struct globals *GLOBALS, char *buffer)
{

  char *buffer_copy = strdup(buffer);
  char *semicolon_token = strtok(buffer_copy, ";");

  if (strcmp(buffer, semicolon_token) != 0)
  {
    determine_what_to_do(GLOBALS, semicolon_token);
    while ((semicolon_token = strtok(NULL, ";")) != NULL)
    {
      // printf("%s\n", semicolon_token);
      determine_what_to_do(GLOBALS, semicolon_token);
    }
    printf("HELLO\n");
  }

  free(buffer_copy); // TODO

  char *buffer_other_copy = strdup(buffer);
  // printf("%s\n", buffer);
  struct array tokens = create_array(sizeof(char *));
  tokenize(buffer_other_copy, " ", &tokens); // &mut tokens

  int index_of_redir = -1;
  for (int i = 0; i < tokens.size; i += 1)
  {
    if (strcmp(*((char **)get_from_array(&tokens, 0)), ">") == 0)
    {
      index_of_redir = i;
      break;
    }
  }

  if (false)
  {
    ;
  }
  else
  { // normal stdin and stdout
    do_commands(&tokens, GLOBALS, stdin, stdout, stderr);
  }
  free(tokens.array_ptr);
  free(buffer_other_copy);
}

void do_commands(struct array *tokens, struct globals *GLOBALS, FILE *sin, FILE *sout, FILE *serr)
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
    attempt_evaluate_from_path(tokens, GLOBALS);
  }
}

void attempt_evaluate_from_path(struct array *tokens, struct globals *GLOBALS)
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
  if (!f) //child
  {
    if (access(*(char **)get_from_array(tokens, 0), F_OK | X_OK) == 0)
    { //attempt to execute the absolute path
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
  wait(NULL); //wait for child process to terminate

  // cleanup
  for (int i = 0; i < paths.size; i += 1)
  {
    free(*(void **)get_from_array(&paths, i));
  }
  free(paths.array_ptr);
  free(path_copy);
  // free(concat);
}