#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <limits.h>
// #define _POSIX_C_SOURCE 1

static char *PATHS = NULL;

char *is_executable(char *location, char *command)
{
    char *path = NULL;
    asprintf(&path, "%s/%s", location, command);
    if (access(path, F_OK | X_OK) ==
        0)
    {
        return path;
    }
    return NULL;
}

char *get_absolute_path(char *command)
{

    char *strtok_state = NULL;
    char *paths_copy = strdup(PATHS);
    for (char *token = strtok_r(paths_copy, ":", &strtok_state); token != NULL; token = strtok_r(NULL, ":", &strtok_state))
    {
        char *executable_path = is_executable(token, command);
        if (executable_path != NULL)
        {
            return executable_path;
        }
    }
    free(paths_copy);

    // try current directory
    char cwd[PATH_MAX];
    if (getcwd(cwd, PATH_MAX) != NULL)
    {
        char *executable_path = is_executable(cwd, command);
        if (executable_path != NULL)
        {
            return executable_path;
        }
    }
    else
    {
        perror("getcwd() error");
    }

    // try absolute path
    char *c = strdup(command);
    if (access(c, F_OK | X_OK) ==
        0)
    {
        return c;
    }

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
