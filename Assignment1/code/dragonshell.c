#define _GNU_SOURCE
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "welcome.h"
#include "commands.h"

// #define BUFFER_SIZE 32;

/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize(char* str, const char* delim, char ** argv) {
  char* token;
  token = strtok(str, delim);
  size_t i;
  for(i = 0; token != NULL; ++i){
    argv[i] = token;
    token = strtok(NULL, delim);
  }
  argv[i] = NULL;
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

    char **tokens = malloc(64 * sizeof(char*)); // could dynamically allocate it

    tokenize(buffer, " ", tokens);
    
    do_commands(tokens);
    // printf("%d", tokens[2]);

    free(buffer);
    free(tokens);
    break;
    
  }
  
  return 0;
}