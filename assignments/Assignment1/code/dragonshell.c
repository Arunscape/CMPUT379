#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "array.h"
#include "commands.h"
#include "util.h"
#include "welcome.h"
#include "paths.h"
int main(int argc, char **argv)
{

  add_to_path("/bin/:/usr/bin");
  handle_signals();

  printDragon();

  for (;;)
  {

    printf("\n🐲 dragonshell🐉 > ");

    char *buffer = NULL;
    size_t buffer_size = 0;
    if (getline(&buffer, &buffer_size, stdin) == -1)
    {
      break;
    }; // &mut buffer &mut buffer_size
    strtok(buffer, "\n");
    // get rid of \n at the end of the line
    run_line(buffer); // &mut buffer
    free(buffer);

    // _exit(0);
    // exit(0);
    if (false)
    {
      free(buffer);
    }
  }
  // return 0;
  cleanup_PATH();
  _exit(0);
}