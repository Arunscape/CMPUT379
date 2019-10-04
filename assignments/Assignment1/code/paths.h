#ifndef PATHS_H_
#define PATHS_H_

void add_to_path(const char *path);
void cleanup_PATH();
void print_path();
char *get_absolute_path(char *command);

#endif // PATHS_H_