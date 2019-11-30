#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>

#include "FileSystem.h"
#include "util.h"

// globals
Super_block *SUPER_BLOCK = NULL;

int DISK_FD = -1;
int8_t CWD = -1;

void fs_mount(char *new_disk_name) {
  if (attempt_mount(new_disk_name)) {
    // successfully mounted
    return;
  }

  // unsuccessful mount, see if filesystem was mounted before
  if (DISK_FD == -1)
    fprintf(stderr, "Error: No file system is mounted\n");
}

void fs_create(char name[5], int size) {
  if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
    // TODO ask TA what the error message should be
    fprintf(stderr, ". and .. are reserved names\n");
    return;
  }

  bool free_block = false;
  for (uint8_t i = 0; i < 126; i += 1) {
    if (inode_is_free(SUPER_BLOCK->inode[i])) {
      free_block = true;
      break;
    }
  }

  if (!free_block) {
    fprintf(stderr,
            "Error: Superblock in <disk name> is full, cannot create %s\n",
            name);
    return;
  }

  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];

    if (inode_parent(inode) != CWD)
      continue;

    if (strcmp(name, inode.name) == 0) {
      fprintf(stderr, "Error: File or directory %s already exists\n", name);
      return;
    }
  }

  if (size > 0) { // file

    int8_t available_block = -1;
    for (uint8_t i = 0; i < 126; i += 1) {
      Inode inode = SUPER_BLOCK->inode[i];

      if (!inode_is_free(inode))
        continue;

      bool candidate_works = true;
      for (uint8_t j = i; j < i + size; i += 1) {
        if (inode_in_use(inode)) {
          candidate_works = false;
          break;
        }
      }

      if (candidate_works) {
        available_block = i;
        break;
      }
    }

    if (available_block < 0) {
      fprintf(stderr, "Cannot allocate %d, on <disk name>\n", size); // TODO
      return;
    }

    printf("CREATING A FILE\n");

    Inode starting_block = SUPER_BLOCK->inode[available_block];
    strncpy(starting_block.name, name, 5);
    starting_block.used_size = 0b10000000 | size;
    starting_block.dir_parent = CWD;

    for (uint8_t i = available_block + 1; i < available_block + size; i += 1) {
      SUPER_BLOCK->inode[i].used_size = 0b10000000;
    }
    return;
  }

  if (size == 0) {
    int8_t available_block = -1;
    for (uint8_t i = 0; i < 126; i += 1) {
      if (inode_is_free(SUPER_BLOCK->inode[i]))
        available_block = i;
    }
    if (available_block < 0) {
      fprintf(stderr, "Cannot allocate %d, on <disk name>\n", size);
      return;
    }

    printf("IDK WHAT IS HAPPENING\n");

    Inode starting_block = SUPER_BLOCK->inode[available_block];
    strncpy(starting_block.name, name, 5);
    starting_block.used_size = size;
    starting_block.dir_parent = CWD;
    return;
  }
}
void fs_delete(char name[5]) {
  for (size_t i = 0; i < 126; i += 1) {
    if (strcmp(name, SUPER_BLOCK->inode[i].name) == 0) {
      // delete it
      return;
    }
  }
  fprintf(stderr, "File or directory %s does not exist", name);
}
void fs_read(char name[5], int block_num) {}
void fs_write(char name[5], int block_num) {}
void fs_buff(uint8_t buff[1024]) {}
void fs_ls(void) {}
void fs_resize(char name[5], int new_size) {}
void fs_defrag(void) {}
void fs_cd(char name[5]) {}
