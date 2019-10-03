#ifndef GLOBALS_H_
#define GLOBALS_H_
#include <stdbool.h>

struct globals
{
  char *PATH;
  bool EXIT;
};
#endif // GLOBALS_H_

// TODO store pid of child processes in global struct
// it seems that there can be at most 4 processes at the same time