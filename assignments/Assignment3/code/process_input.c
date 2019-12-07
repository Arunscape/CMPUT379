#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "FileSystem.h"

extern int DISK_FD;
extern Super_block *SUPER_BLOCK;
extern char *DISK_NAME;

void run_command(char *line, size_t line_number, const char *input_file_name);

void process_input(const char *file) {

  FILE *f;
  char *line = NULL;
  size_t len = 0;
  ssize_t line_length;

  f = fopen(file, "r");
  if (f == NULL) {
    fprintf(stderr, "Error: Could not read input file %s\n", file);
  }

  size_t line_number = 0;
  while ((line_length = getline(&line, &len, f)) != -1) {
    // fprintf(stdout, "Line length: %zu\n", line_length);
    // fprintf(stdout, "%s\n", line);

    // strip \n
    line = strtok(line, "\n");
    line_number += 1;
    run_command(line, line_number, file); // &mut line
  }

  free(line);
  fclose(f);
  close(DISK_FD);
  free(SUPER_BLOCK);
  free(DISK_NAME);
}

void command_error(const char *input_file_name, size_t line_number) {
  fprintf(stderr, "Command Error: %s, %zu\n", input_file_name, line_number);
}

void error_no_filesystem_mounted() {
  fprintf(stderr, "Error: No file system is mounted\n");
}

void run_command(char *line, size_t line_number, const char *input_file_name) {

  char *strtok_state = NULL;
  char *first_token = strtok_r(line, " ", &strtok_state);

  if (strcmp(first_token, "M") == 0) {

    // not enough arguments
    char *new_disk_name;
    if ((new_disk_name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      printf("not enough arguments for mount\n");
      return;
    }

    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      printf("too many arguments for mount\n");
      return;
    }

    fs_mount(new_disk_name);
  } else if (strcmp(first_token, "C") == 0) {

    // file name not provided
    char *name;
    if ((name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      printf("file name not provided for create\n");
      return;
    }

    // file size not provided
    char *size_str;
    if ((size_str = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("file size not provided for create\n");
      return;
    }

    char *endptr = NULL;
    errno = 0;
    long str_length = strtol(size_str, &endptr, 10);
    int size = -1;
    if (0 <= str_length && str_length <= INT_MAX) {
      size = str_length;
    } else {
      fprintf(stderr, "file size exceeds size of int lol");
    }

    // unable to parse integer
    if (errno != 0 || size < 0 || size > 127) {
      command_error(input_file_name, line_number);
      // printf("cannot parse integer in create\n");
      return;
    }

    // errno == 0 at this point
    // leftover characters after strtol
    if (*endptr != '\0') {
      command_error(input_file_name, line_number);
      // printf("there's extra stuff\n");
      return;
    }

    // file name too long
    if (strlen(name) > 5) {
      command_error(input_file_name, line_number);
      //      printf("file name too long\n");
      return;
    }

    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      // printf("too many arguments for create file\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }

    fs_create(name, size);
  } else if (strcmp(first_token, "D") == 0) {
    char *name;
    if ((name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("file name not provided for delete\n");
      return;
    }
    // file name too long
    if (strlen(name) > 5) {
      command_error(input_file_name, line_number);
      // printf("file name too long\n");
      return;
    }

    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      // printf("too many arguments for delete file\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }
    fs_delete(name);
  } else if (strcmp(first_token, "R") == 0) {

    // file name not provided
    char *name;
    if ((name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("file name not provided for read\n");
      return;
    }

    // file size not provided
    char *block_num_str;
    if ((block_num_str = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("block_num not provided for read\n");
      return;
    }

    char *endptr = NULL;
    errno = 0;
    long str_block_num = strtol(block_num_str, &endptr, 10);
    int block_num = -1;
    if (0 <= str_block_num && str_block_num <= INT_MAX) {
      block_num = str_block_num;
    } else {
      // fprintf(stderr, "block num exceeds size of int lol");
    }

    // unable to parse integer
    if (errno != 0 || block_num < 0) {
      command_error(input_file_name, line_number);
      // printf("cannot parse integer in create\n");
      return;
    }

    // errno == 0 at this point
    // leftover characters after strtol
    if (*endptr != '\0') {
      command_error(input_file_name, line_number);
      // printf("there's extra stuff\n");
      return;
    }

    // file name too long
    if (strlen(name) > 5) {
      command_error(input_file_name, line_number);
      // printf("file name too long\n");
      return;
    }

    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      // printf("too many arguments for read file\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }

    fs_read(name, block_num);
  } else if (strcmp(first_token, "W") == 0) {
    // file name not provided
    char *name;
    if ((name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("file name not provided for write\n");
      return;
    }

    // file size not provided
    char *block_num_str;
    if ((block_num_str = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("block_num not provided for write\n");
      return;
    }

    char *endptr = NULL;
    errno = 0;
    long str_block_num = strtol(block_num_str, &endptr, 10);
    int block_num = -1;
    if (0 <= str_block_num && str_block_num <= INT_MAX) {
      block_num = str_block_num;
    } else {
      // fprintf(stderr, "block num exceeds size of int lol");
    }

    // unable to parse integer
    if (errno != 0 || block_num < 0) {
      command_error(input_file_name, line_number);
      // printf("cannot parse integer in write\n");
      return;
    }

    // errno == 0 at this point
    // leftover characters after strtol
    if (*endptr != '\0') {
      command_error(input_file_name, line_number);
      // printf("there's extra stuff\n");
      return;
    }

    // file name too long
    if (strlen(name) > 5) {
      command_error(input_file_name, line_number);
      // printf("file name too long\n");
      return;
    }

    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      // printf("too many arguments for write file\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }
    fs_write(name, block_num);
  } else if (strcmp(first_token, "B") == 0) {
    uint8_t buff[1024] = {0};
    char *buff_str;
    if ((buff_str = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      //      printf("data not provided for buffer\n");
      return;
    }

    // undo what strtok did
    strtok_state[-1] = ' ';
    //    depends on glibc implementation
    //    https://github.com/bminor/glibc/blob/5cb226d7e4e710939cff7288bf9970cb52ec0dfa/string/strtok_r.c#L73

    // file name too long
    if (strlen(buff_str) > 1024) {
      command_error(input_file_name, line_number);
      //      printf("too much data provided for buffer\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }

    strncpy((char *)buff, buff_str, 1024);
    fs_buff(buff);
  } else if (strcmp(first_token, "L") == 0) {
    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      //      printf("too many arguments for ls\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }
    fs_ls();
  } else if (strcmp(first_token, "E") == 0) {
    // resize
    // file name not provided
    char *name;
    if ((name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("file name not provided for resize\n");
      return;
    }

    // file size not provided
    char *size_str;
    if ((size_str = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("file size not provided for resize\n");
      return;
    }

    char *endptr = NULL;
    errno = 0;
    long str_length = strtol(size_str, &endptr, 10);
    int size = -1;
    if (0 <= str_length && str_length <= INT_MAX) {
      size = str_length;
    } else {
      // fprintf(stderr, "file size exceeds size of int lol");
    }

    // unable to parse integer
    if (errno != 0 || size < 0) {
      command_error(input_file_name, line_number);
      printf("cannot parse integer in resize\n");
      return;
    }

    // errno == 0 at this point
    // leftover characters after strtol
    if (*endptr != '\0') {
      command_error(input_file_name, line_number);
      // printf("there's extra stuff\n");
      return;
    }

    // file name too long
    if (strlen(name) > 5) {
      command_error(input_file_name, line_number);
      // printf("file name too long\n");
      return;
    }

    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      // printf("too many arguments for resize file\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }
    fs_resize(name, size);
  } else if (strcmp(first_token, "O") == 0) {
    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      // printf("too many arguments for resize file\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }

    fs_defrag();
  } else if (strcmp(first_token, "Y") == 0) {
    char *name;
    if ((name = strtok_r(NULL, " ", &strtok_state)) == NULL) {
      command_error(input_file_name, line_number);
      // printf("file name not provided for delete\n");
      return;
    }
    // file name too long
    if (strlen(name) > 5) {
      command_error(input_file_name, line_number);
      // printf("file name too long\n");
      return;
    }

    // too many arguments
    if (strtok_r(NULL, " ", &strtok_state) != NULL) {
      command_error(input_file_name, line_number);
      // printf("too many arguments for delete file\n");
      return;
    }

    if (DISK_FD == -1) {
      error_no_filesystem_mounted();
      return;
    }
    fs_cd(name);
  } else
    command_error(input_file_name, line_number);
}
