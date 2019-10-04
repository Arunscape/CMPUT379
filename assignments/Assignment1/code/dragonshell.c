#define _GNU_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <errno.h>

#include "array.h"
#include "commands.h"
#include "util.h"
#include "welcome.h"
#include "paths.h"

int main(int argc, char **argv)
{

  struct array CHILDREN_PIDS = create_array(sizeof(pid_t));

  
  handle_signals();
  add_to_path("/bin/:/usr/bin");

  printDragon();

  for (;;)
  {

    printf("SIZE: %zu\n", CHILDREN_PIDS.size);
    printf("🐲 dragonshell🐉 > ");

    char *buffer = NULL;
    size_t buffer_size = 0;
    if (getline(&buffer, &buffer_size, stdin) == -1)
    {
      if (feof(stdin))
      {
        // printf("REEEE\n");
        clearerr(stdin);
        // kill_children(&CHILDREN_PIDS);
        break;
      }

      free(buffer);
      continue;
    };
    strtok(buffer, "\n");                  // get rid of \n at the end of the line
    if (!run_line(buffer, &CHILDREN_PIDS)) // run the line. if false is returned, exit
    {
      free(buffer);
      // TODO other cleanup
      break;
    };
    free(buffer);
  }
  // TODO other cleanup;
  printf("REEEE\n");
  cleanup_PATH();
  kill_children(&CHILDREN_PIDS);
  _exit(0);
}