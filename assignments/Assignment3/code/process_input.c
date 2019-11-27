#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "FileSystem.h"

void run_command(char *line, size_t line_number, const char *input_file_name);

void process_input(const char *file) {

  FILE *f;
  char *line = NULL;
  size_t len = 0;
  ssize_t line_length;

  f = fopen(file, "r");
  if (f == NULL) {
    fprintf(stderr, "Error: Could not read input file %s", file);
  }

  size_t line_number = 0;
  while ((line_length = getline(&line, &len, f)) != -1) {
    // fprintf(stdout, "Line length: %zu\n", line_length);
    // fprintf(stdout, "%s\n", line);

    // strip \n
    line = strtok(line, "\n");
    run_command(line, line_number, file); // &mut line
    line_number += 1;
  }

  free(line);
  fclose(f);
}

void command_error(const char *input_file_name, size_t line_number) {
  fprintf(stderr, "Command Error: %s, %zu\n", input_file_name, line_number);
}

void run_command(char *line, size_t line_number, const char *input_file_name) {

  char *strtok_state = NULL;
  char *first_token = strtok_r(line, " ", &strtok_state);

  if (strcmp(first_token, "M") == 0) {

    // not enough arguments
    char *new_disk_name;
    if ((new_disk_name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      return;
    }

    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      return;
    }

    fs_mount(new_disk_name);
  } else if (strcmp(first_token, "C") == 0) {

    // file name not provided
    char *name;
    if ((name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      return;
    }

    // file size not provided
    char *size_str;
    if ((name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      return;
    }

    char *endptr = NULL;
    errno = 0;
    int size = strtol(size_str, &endptr, 10);

    // unable to parse integer
    if (errno != 0) {
      command_error(input_file_name, line_number);
      return;
    }

    // errno == 0 at this point
    // leftover characters after strtol
    if (size_str && *endptr != 0) {
      command_error(input_file_name, line_number);
      return;
    }

    // file name too long
    if (strlen(name) > 5) {
      command_error(input_file_name, line_number);
    }

    fs_create(name, size);
  } else if (strcmp(first_token, "D") == 0) {
    char name[5];
    fs_delete(name);
  } else if (strcmp(first_token, "R") == 0) {
    char name[5];
    int block_num;
    fs_read(name, block_num);
  } else if (strcmp(first_token, "W") == 0) {
    char name[5];
    int block_num;
    fs_write(name, block_num);
  } else if (strcmp(first_token, "B") == 0) {
    uint8_t buff[1024];
    fs_buff(buff);
  } else if (strcmp(first_token, "L") == 0) {
    fs_ls();
  } else if (strcmp(first_token, "E") == 0) {
    char name[5];
    int new_size;
    fs_resize(name, new_size);
  } else if (strcmp(first_token, "O") == 0) {
    fs_defrag();
  } else if (strcmp(first_token, "Y") == 0) {
    char name[5];
    fs_cd(name);
  } else
    command_error(input_file_name, line_number);
}
