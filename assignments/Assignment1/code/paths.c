#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static char *PATHS = NULL;

char *get_absolute_path(char *command)
{

    char *strtok_state = NULL;
    char *paths_copy = strdup(PATHS);
    for (char *token = strtok_r(paths_copy, ":", &strtok_state); token != NULL; token = strtok_r(NULL, ":", &strtok_state))
    {
        char *path = NULL;
        asprintf(&path, "%s/%s", token, command);
        if (access(path, F_OK | X_OK) ==
            0)
        {
            free(paths_copy);
            return path;
        }
    }
    free(paths_copy);
    return NULL;
    // TODO check current directory
}

void add_to_path(char *path)
{
    if (PATHS == NULL)
    {
        PATHS = strdup(path);
    }
    else
    {
        char *tmp = PATHS;
        PATHS = NULL;
        asprintf(&PATHS, "%s:%s", tmp, path);
        free(tmp);
    }
}

void print_path()
{
    fprintf(stdout, "%s", PATHS);
}

void cleanup_PATH()
{
    free(PATHS);
}
