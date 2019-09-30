#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "array.h"
#include "commands.h"
#include "welcome.h"

// tokenize function modified to work with the custom c array
// we're storing pointers to the char** strings in the array
void tokenize(char *str, const char *delim, struct array *array)
{
  char *token = strtok(str, delim);
  for (size_t i = 0; token != NULL; i += 1)
  {
    push_to_array(array, &token);
    token = strtok(NULL, delim);
  }
}

int main(int argc, char **argv)
{
  printDragon();

  for (;;)
  {

    printf("🐲dragonshell🐉> ");

    char *buffer = NULL;
    size_t buffer_size = 0;
    getline(&buffer, &buffer_size, stdin);

    struct array tokens = create_array(sizeof(char *));
    tokenize(buffer, " ", &tokens); // &mut
    do_commands(&tokens);

    // for (int i = 0; i < tokens.size; i++)
    // {
    //   printf("%s\n", *((char **)get_from_array(&tokens, i)));
    // }

    free(buffer);
    free(tokens.array_ptr);
    break;
  }

  return 0;
}