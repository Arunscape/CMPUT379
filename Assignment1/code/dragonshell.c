#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "welcome.h"

// #define BUFFER_SIZE 32;

/**
 * @brief Tokenize a C string
 *
 * @param str - The C string to tokenize
 * @param delim - The C string containing delimiter character(s)
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
char **tokenize(char *str, const char *delim) {

  int len = strlen(str);
  int num_of_spaces = 0;
  for (int i = 0; i < len; i++) {
    if (strcmp(str[i], ' ') == 0) {
      num_of_spaces += 1;
    }
  }

  char **tokens = malloc((num_of_spaces + 1) * sizeof(char *));
  char *token;
  token = strtok(str, delim);
  size_t i;
  for (i = 0; token != NULL; ++i) {
    tokens[i] = token;
    token = strtok(NULL, delim);
  }
  tokens[i] = NULL;

  return tokens;
}

int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop

  printDragon();

  for (;;) {

    printf("🐲dragonshell🐉> ");

    char *buffer = NULL;
    size_t buffer_size = 0;

    getline(&buffer, &buffer_size, stdin);

    // char **tokens =
    //     malloc(64 * sizeof(char *)); // dragonshell will have a limit of 64
    //     commands

    char **tokens = tokenize(buffer, " ");

    fprintf("%s", tokens[0]);
    // do_commands(tokens);

    free(buffer);
    free(tokens);
    break;
  }

  return 0;
}