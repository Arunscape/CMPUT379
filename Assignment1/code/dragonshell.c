#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "array.h"
#include "commands.h"
#include "welcome.h"

/**
 * @brief Tokenize a C string
 *
 * @param str - The C string to tokenize
 * @param delim - The C string containing delimiter character(s)
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize(char *str, const char *delim, struct array *array)
{
  char *token = strtok(str, delim);
  for (size_t i = 0; token != NULL; i += 1)
  {
    push_to_array(array, token);
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

    struct array tokens = create_array(64);
    tokenize(buffer, " ", &tokens);
    // do_commands(idk);

    for (int i = 0; i < tokens.size; i++)
    {
      printf("%s\n", get_from_array(&tokens, i));
    }

    free(buffer);
    // free(tokens);
    break;
  }

  return 0;
}