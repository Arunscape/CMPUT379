#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <sys/types.h>

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
  // if (DISK_FD == -1)
  //  fprintf(stderr, "Error: No file system is mounted\n");
}

void fs_create(char name[5], int size) {
  if (no_filesystem_mounted()) {
    return;
  }

  if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
    // TODO ask TA what the error message should be
    fprintf(stderr, ". and .. are reserved names\n");
    return;
  }

  // you should check the availability of a free inode
  uint8_t first_available_inode = get_first_available_inode();
  // printf("WRITING TO INODE %u\n", first_available_inode);
  if (first_available_inode < 0) {

    printf("create: NO INODES AVAILABLE\n"); // TODO

    fprintf(stderr,
            "Error: Superblock in <disk name> is full, cannot create %s\n",
            name);
    return;
  }

  if (is_a_duplicate(name)) {
    fprintf(stderr, "Error: File or directory %s already exists\n", name);
    return;
  }

  int8_t first_available_block = get_first_available_block();
  if (first_available_block < 0) {
    printf("create: NO BLOCKS AVAILABLE\n"); // TODO
    fprintf(stderr, "Error: Cannot allocate %d, on <TODO GET DISK NAME>", size);
    return;
  }

  if (size > 0) { // file

    uint8_t start_block = 255;
    for (uint8_t candidate = first_available_block; candidate < 128;
         candidate += 1) {
      bool candidate_works = true;
      for (uint8_t i = candidate; i < candidate + size; i += 1) {
        if (block_in_use(i)) {
          candidate_works = false;
          fprintf(stderr, "Block %d in use\n", i); // TODO
          break;
        }
      }

      if (candidate_works) {
        start_block = candidate;
        break;
      }
    }

    if (start_block > 127 || (start_block + size - 1 > 127)) {
      fprintf(stderr, "Error: Cannot allocate %d, on <TODO GET DISK NAME>",
              size);
      return;
    }

    update_inode(first_available_inode, name, size, start_block, CWD, true,
                 false);

    update_blocks(start_block, start_block + size, true);

    write_superblock();

    return;
  }

  if (size == 0) { // directory

    update_inode(first_available_inode, name, size, 0, CWD, true, true);
    write_superblock();
    return;
  }
}
void fs_delete(char name[5]) {

  bool did_not_delete = true;
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode *inode = &SUPER_BLOCK->inode[i];

    if (inode_parent(*inode) != CWD)
      continue;

    if (strncmp(inode->name, name, 5) != 0)
      continue;

    if (inode_is_file(*inode)) {
      delete_inode(inode);
      did_not_delete = false;
      break;
    }
    if (inode_is_directory(*inode)) {
      recursive_delete_inode(inode);
      did_not_delete = false;
      break;
    }
  }

  if (did_not_delete)
    fprintf(stderr, "File or directory %s does not exist\n", name);
}
void fs_read(char name[5], int block_num) {}
void fs_write(char name[5], int block_num) {}
void fs_buff(uint8_t buff[1024]) {}
void fs_ls(void) {}
void fs_resize(char name[5], int new_size) {}
void fs_defrag(void) {}
void fs_cd(char name[5]) {}
