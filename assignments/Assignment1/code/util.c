#include <string.h>
#include <unistd.h>

#include "array.h"

// tokenize function modified to work with the custom c array
// we're storing pointers to the char** strings in the array
void tokenize(char *str, const char *delim, struct array *array)
{
  char *token_state = NULL;
  char *token = strtok_r(str, delim, &token_state);
  printf("token: %s *\n", token);
  for (size_t i = 0; token != NULL; i += 1)
  {
    push_to_array(array, &token);
    token = strtok_r(NULL, delim, &token_state);
    printf("token: %s *\n", token);
  }
}