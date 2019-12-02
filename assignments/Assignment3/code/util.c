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

void seek_beginning_file() {
  if (lseek(DISK_FD, 0, SEEK_SET) < -1) {
    perror("error seeking to beginning of file");
  }
}

void load_superblock() {
  if (read(DISK_FD, SUPER_BLOCK, sizeof(Super_block)) < 0) {
    perror("error reading superblock");
  }
  seek_beginning_file();
}

bool no_filesystem_mounted() {
  if (DISK_FD == -1) {
    fprintf(stderr, "Error: No file system is mounted\n");
    return true;
  }
  return false;
}

bool attempt_mount(char *new_disk_name) {
  // check if a virtual disk exists with the given name in the current directory
  // if not,
  int old_disk_fd = DISK_FD;

  DISK_FD = open(new_disk_name, O_RDWR);
  if (DISK_FD < 0) {
    perror("delete me");
    fprintf(stderr, "Error: Cannot find disk %s\n", new_disk_name);
    return false;
  }

  SUPER_BLOCK = malloc(sizeof(Super_block)); // TODO free

  load_superblock();

  // check for consistency of filesystem
  int8_t error_code = do_checks();
  if (error_code != 0) {
    fprintf(stderr,
            "Error: File system in %s is inconsistent (error code: %hhd)\n",
            new_disk_name, error_code);
    // use the last filesystem that was mounted
    DISK_FD = old_disk_fd;
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
  if (write(DISK_FD, SUPER_BLOCK, sizeof(Super_block)) < 0) {
    fprintf(stderr, "WHAT");
  }
  seek_beginning_file();
}

bool block_in_use(uint8_t i) {
  uint8_t index = i / 8;
  uint8_t shift = 7 - (i % 8);
  uint8_t in_use = (SUPER_BLOCK->free_block_list[index] >> shift) & 1;

  // printf("block: %d, %u, index %d in use: %d\n",
  //    index, (uint8_t) SUPER_BLOCK->free_block_list[index], i, in_use);
  return in_use;
}

bool block_is_free(uint8_t i) { return !block_in_use(i); }

int8_t get_first_available_block() {
  for (uint8_t i = 0; i < 128; i += 1) {
    // printf("block %d is free: %d\n", i, block_is_free(i));
    if (block_is_free(i))
      return i;
  }
  return -1; // no blocks available
}

int8_t get_first_available_inode() {
  for (uint8_t i = 0; i < 126; i += 1) {
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

void update_inode(uint8_t i, char name[5], uint8_t size, uint8_t start_block,
                  uint8_t dir_parent, bool used, bool is_directory) {

  fprintf(stderr, "INODE SIZE SET TO %u\n", size);

  Inode *inode = &SUPER_BLOCK->inode[i];
  strncpy(inode->name, name, 5);
  inode->used_size = size;
  inode->start_block = start_block;
  inode->dir_parent = dir_parent;

  if (used)
    inode->used_size = inode->used_size | 0b10000000;

  fprintf(stderr, "INODE SIZE IS NOW %u\n", inode->used_size);
  if (is_directory)
    inode->dir_parent = inode->dir_parent | 0b10000000;

  /*
  printf("inode being updated: %d\n", i);
  printf("name: %s\n", inode->name);
  printf("size: %d\n", inode->used_size);
  printf("start block: %d\n", inode->start_block);
  printf("parent dir: %d\n", inode->dir_parent);
  */
}

void update_blocks(uint8_t start, uint8_t end, bool set) {
  fprintf(stderr, "SETTING BLOCKS %u to %u to %d\n", start, end -1, set);
  for (uint8_t i = start; i < end; i += 1) {
    uint8_t index = i / 8;
    uint8_t bitmask = 1 << (7 - (i % 8));
    if (set) {
      //
      // printf("bit: %i, bitmask: %u\n", i, bitmask);
      SUPER_BLOCK->free_block_list[index] =
          SUPER_BLOCK->free_block_list[index] | bitmask;
      // uint8_t idk = (uint8_t) SUPER_BLOCK->free_block_list[index];
      // printf("setting block %d to %u\n", index, idk);
    } else {
      bitmask = bitmask ^ 0b11111111;
      SUPER_BLOCK->free_block_list[index] =
          SUPER_BLOCK->free_block_list[index] & bitmask;
    }
  }

  // for (int i=0; i<16; i+=1)
  //  printf("block %d : %u\n", i, (uint8_t) SUPER_BLOCK->free_block_list[i]);
}
