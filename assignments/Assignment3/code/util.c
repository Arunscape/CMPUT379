#define _GNU_SOURCE
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FileSystem.h"
#include "consistency_checks.h"

extern int DISK_FD;
extern int CWD;
extern Super_block *SUPER_BLOCK;

bool attempt_mount(char *new_disk_name) {
  // check if a virtual disk exists with the given name in the current directory
  // if not,
  DISK_FD = open(new_disk_name, O_RDWR);
  if (DISK_FD < 0) {
    perror("delete me");
    fprintf(stderr, "Error: Cannot find disk %s\n", new_disk_name);
    return false;
  }

  SUPER_BLOCK = malloc(sizeof(Super_block)); // TODO free
  if (read(DISK_FD, SUPER_BLOCK, 1024) < 0) {
    perror("uhhh");
    fprintf(stderr, "ERROR REEADING SUPER BLOCK\n");
  }

  // check for consistency of filesystem
  int8_t error_code = do_checks();
  if (error_code != 0) {
    fprintf(stderr,
            "Error: File system in %s is inconsistent (error code: %i)\n",
            new_disk_name, error_code);
    // use the last filesystem that was mounted

    return false;
  }

  // set cwd to /
  CWD = 127;

  return true;
}

uint8_t inode_parent(Inode inode) { return inode.dir_parent & 0b01111111; }

bool inode_in_use(Inode inode) { return (inode.used_size >> 7) & 1; }
bool inode_is_free(Inode inode) { return !inode_in_use(inode); }

bool inode_is_directory(Inode inode) { return (inode.dir_parent >> 7) & 1; }
bool inode_is_file(Inode inode) { return !inode_is_directory(inode); }

void write_superblock() {
  if (write(DISK_FD, SUPER_BLOCK, 1024) < 0) {
    fprintf(stderr, "WHAT");
  }
}

bool block_is_free(int8_t i) {
  int8_t index = i / 8;
  int8_t shift = 7 - (i % 8);
  int8_t in_use = (SUPER_BLOCK->free_block_list[index] >> shift) & 1;

  return in_use;
}

bool block_in_use(int8_t i) { return !block_is_free(i); }

int8_t get_first_available_block() {
  for (int8_t i = 1; i < 128; i += 1) {
    if (block_is_free(i))
      return i;
  }
  return -1; // no blocks available
}

int8_t get_first_available_inode() {
  for (int8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];
    if (inode_is_free(inode))
      return i;
  }
  return -1; // no inodes available
}

bool is_a_duplicate(char *name) {
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];

    if (inode_parent(inode) != CWD)
      continue;

    if (strcmp(name, inode.name) == 0)
      return true;
  }
  return false;
}

void update_inode(int8_t i, char name[5], uint8_t size, uint8_t start_block,
                  uint8_t dir_parent, bool used, bool is_directory) {
  Inode *inode = &SUPER_BLOCK->inode[i];
  strncpy(inode->name, name, 5);
  inode->used_size = size;
  inode->start_block = start_block;
  inode->dir_parent = dir_parent;

  if (used)
    inode->used_size = inode->used_size | 0b10000000;

  if (is_directory)
    inode->dir_parent = inode->dir_parent | 0b10000000;
}

void update_blocks(uint8_t start, uint8_t end, bool set) {
  for (uint8_t i = start; i < end; i += 1) {
    uint8_t index = i / 8;
    uint8_t bitmask = 1 << (i % 8);
    if (set) {
      SUPER_BLOCK->free_block_list[index] =
          SUPER_BLOCK->free_block_list[index] | bitmask;
    } else {
      bitmask = bitmask ^ 0b11111111;
      SUPER_BLOCK->free_block_list[index] =
          SUPER_BLOCK->free_block_list[index] & bitmask;
    }
  }
}
