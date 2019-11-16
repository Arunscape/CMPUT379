#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "FileSystem.h"

void run_command(char* line);

void process_input(const char* file) {

  FILE *f;
	char *line = NULL;
	size_t len = 0;
	ssize_t line_length;
 
	f = fopen(file, "r");
	if (f == NULL) {
    fprintf(stderr, "Error: Could not read input file %s", file);
  }
 
	while ((line_length = getline(&line, &len, f)) != -1) {
	  // fprintf(stdout, "Line length: %zu\n", line_length);
    // fprintf(stdout, "%s\n", line);
    run_command(line); // &mut line
  }
 
	free(line);
	fclose(f);
}

void run_command(char* line) {
  
  char first_character = *line;

  if (first_character == 'M') {
    char* new_disk_name;
    fs_mount(new_disk_name);
  }
  else if (first_character == 'C') {
    char name[5];
    int size;
    fs_create(name, size);
  }
  else if (first_character == 'D') {
    char name[5];
    fs_delete(name);
  }
  else if (first_character == 'R') {
    char name[5];
    int block_num;
    fs_read(name, block_num);
  }
  else if (first_character == 'W') {
    char name[5];
    int block_num;
    fs_write(name, block_num);
  }
  else if (first_character == 'B') {
    uint8_t buff[1024];
    fs_buff(buff);
  }
  else if (first_character == 'L') {
    fs_ls();
  }
  else if (first_character == 'E') {
    char name[5];
    int new_size;
    fs_resize(name, new_size);
  }
  else if (first_character == 'O') {
    fs_defrag();
  }
  else if (first_character == 'Y') {
   char name[5];
   fs_cd(name);
  }
  else
    fprintf(stderr, "Error: Invalid command: %s", line);
}
