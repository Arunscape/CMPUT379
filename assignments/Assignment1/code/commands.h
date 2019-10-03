#ifndef COMMANDS_H_ /* Include guard */
#define COMMANDS_H_

#include "globals.h"

void do_commands(struct array *tokens, struct globals *GLOBALS);

void determine_what_to_do(struct globals *GLOBALS, char *buffer);

#endif // DRAGON_H_