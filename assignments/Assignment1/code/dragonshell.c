#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "array.h"
#include "commands.h"
#include "welcome.h"
#include "util.h"
#include "globals.h"

int main(int argc, char **argv)
{
  struct globals GLOBALS = {
      .PATH = "/bin/:/usr/bin",
  };
  printDragon();

  for (;;)
  {

    printf("🐲 dragonshell🐉 > ");

    char *buffer = NULL;
    size_t buffer_size = 0;
    getline(&buffer, &buffer_size, stdin);
    strtok(buffer, "\n"); // get rid of \n at the end of the line

    struct array tokens = create_array(sizeof(char *));
    tokenize(buffer, " ", &tokens); // &mut
    do_commands(&tokens, &GLOBALS);

    free(buffer);
    free(tokens.array_ptr);
  }

  return 0;
}