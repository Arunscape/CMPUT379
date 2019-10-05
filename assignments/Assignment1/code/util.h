#ifndef UTIL_H_
#define UTIL_H_

void tokenize(char *str, const char *delim, struct array *array);
void handle_signals();

void tok(char *str, const char *delim, char **argv);

void kill_children();

#endif // UTIL_H_