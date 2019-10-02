#include <string.h>
#include <unistd.h>

#include "array.h"

// tokenize function modified to work with the custom c array
// we're storing pointers to the char** strings in the array
void tokenize(char *str, const char *delim, struct array *array) {
  char *token = strtok(str, delim);
  for (size_t i = 0; token != NULL; i += 1) {
    push_to_array(array, &token);
    token = strtok(NULL, delim);
  }
}