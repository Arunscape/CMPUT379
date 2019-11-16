#include <stdio.h>

#include "process_input.h"

int main(int argc, char* argv[]){

  if (argc < 2) {
    fprintf(stderr, "Error: Input file not provided");
    return 1;
  }

  if (argc > 2) {
    fprintf(stderr, "Error: Too many arguments, expected 1 which is the input file");
    return 1;
  }
  
  const char* file = argv[1];
  process_input(file);
  return 0;
}
