#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "array.h"
#include "commands.h"
#include "globals.h"
#include "util.h"
#include "welcome.h"

int main(int argc, char **argv) {
  // struct globals GLOBALS = {
  //     .PATH = (char *)"/bin/:/usr/bin",
  //     .EXIT = 0,
  // };

  struct globals GLOBALS = {
      .PATH = malloc(15 * sizeof(char)),
      .EXIT = false,
  };
  strcpy(GLOBALS.PATH, "/bin/:/usr/bin");
  printDragon();

  for (;;) {

    printf("\n🐲 dragonshell🐉 > ");

    char *buffer = NULL;
    size_t buffer_size = 0;
    getline(&buffer, &buffer_size, stdin); // &mut buffer &mut buffer_size
    strtok(buffer, "\n"); // get rid of \n at the end of the line
    determine_what_to_do(&GLOBALS, buffer); // &mut GLOBALS
    free(buffer);
    if (GLOBALS.EXIT) {
      free(GLOBALS.PATH);
      break;
      // _exit(0);
      // exit(0);
    }
  }
  // return 0;
  _exit(0);
}