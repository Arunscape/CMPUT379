#ifndef COMMANDS_H_ /* Include guard */
#define COMMANDS_H_

#include "globals.h"

void do_commands(struct array *tokens, struct globals *GLOBALS);

void determine_what_to_do(struct array *tokens, struct globals *GLOBALS);

#endif // DRAGON_H_