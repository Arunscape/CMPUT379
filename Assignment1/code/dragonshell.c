#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "dragon.h"

#define BUFFER_SIZE 32;

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
  for(size_t i = 0; token != NULL; ++i){
    argv[i] = token;
  token = strtok(NULL, delim);
  }
}

int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop

  // printDragon();

  for (;;) {

    char* buffer;
    size_t buffer_size = BUFFER_SIZE;

     if( (buffer = malloc(sizeof(char))) == NULL) {
        printf("😭 Couldn't allocate memory");
        exit(1);
    }
    size_t characters = getline(&buffer, &buffer_size, stdin);

    printf(buffer);
    break;
    
  }
  
  return 0;
}



// int main(void) {
//     char *line = NULL;
//     size_t len = 0;
//     ssize_t read = 0;
//     while (read != -1) {
//         puts("enter a line");
//         read = getline(&line, &len, stdin);
//         printf("line = %s", line);
//         printf("line length = %zu\n", read);
//         puts("");
//     }
//     free(line);
//     return 0;
// }