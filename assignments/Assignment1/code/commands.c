#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "array.h"
#include "globals.h"
#include "util.h"

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

  for (int i = 0; i < paths.size; i += 1)
  {
    // printf("%s\n\n", *(void **)get_from_array(&paths, i));
    // TODO figure out how to replace the pointer in the array of pointers to
    // point to the new string
    char *concat = NULL;
    asprintf(&concat, "%s/%s", *(void **)get_from_array(&paths, i),
             *(void **)get_from_array(tokens, 0));

    *(char **)get_from_array(&paths, i) = concat;
  }

  for (int i = 0; i < paths.size; i += 1)
  {
    fprintf(stdout, "%s\n", *(void **)get_from_array(&paths, i));
  }

  //   __pid_t f = fork();
  //   // char *argv[] = {"ls", "-al", NULL};
  //   char *envp[] = {"some", "environment", NULL};
  //   if (f)
  //   {
  //    if (access(filename, F_OK|X_OK) == 0)
  // {
  // }
  //   }
  //   else
  //   {
  //     execve("/bin/ls", argv, envp);
  //   }
  for (int i = 0; i < paths.size; i += 1)
  {
    free(*(void **)get_from_array(&paths, i));
  }
  free(paths.array_ptr);
  free(path_copy);
}