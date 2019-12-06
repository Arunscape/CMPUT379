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
extern uint8_t BUFFER[1024];

void erase_block(uint8_t block);

void load_superblock() {
  if (pread(DISK_FD, SUPER_BLOCK, sizeof(Super_block), 0) < 0) {
    perror("error reading superblock");
  }
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
    // perror("delete me");
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
uint8_t inode_used_size(Inode inode) { return inode.used_size & 0b01111111; }

bool inode_in_cwd(Inode inode) { return inode_parent(inode) == CWD; }

bool inode_not_in_cwd(Inode inode) { return !inode_in_cwd(inode); }

bool inode_in_use(Inode inode) { return (inode.used_size >> 7) & 1; }
bool inode_is_free(Inode inode) { return !inode_in_use(inode); }

bool inode_is_directory(Inode inode) { return (inode.dir_parent >> 7) & 1; }
bool inode_is_file(Inode inode) { return !inode_is_directory(inode); }

void write_superblock() {
  if (pwrite(DISK_FD, SUPER_BLOCK, sizeof(Super_block), 0) < 0) {
    perror("error writing superblock");
  }
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

  // fprintf(stderr, "INODE SIZE SET TO %u\n", size);

  Inode *inode = &SUPER_BLOCK->inode[i];
  strncpy(inode->name, name, 5);
  inode->used_size = size;
  inode->start_block = start_block;
  inode->dir_parent = dir_parent;

  if (used)
    inode->used_size = inode->used_size | 0b10000000;

  // fprintf(stderr, "INODE SIZE IS NOW %u\n", inode->used_size);
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
  // fprintf(stderr, "SETTING BLOCKS %u to %u to %d\n", start, end - 1, set);
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
      // I could've used ~
      bitmask = bitmask ^ 0b11111111;
      SUPER_BLOCK->free_block_list[index] =
          SUPER_BLOCK->free_block_list[index] & bitmask;
      erase_block(i);
    }
  }

  // for (int i=0; i<16; i+=1)
  //  printf("block %d : %u\n", i, (uint8_t) SUPER_BLOCK->free_block_list[i]);
}

void delete_inode(Inode *inode) {

  // if it's not a directory, update the blocks
  if (inode_is_file(*inode)) {
    char name[5];
    strncpy(name, inode->name, 5);
    // printf("TRYING TO DELETE INODE %s\n", name);
    update_blocks(inode->start_block,
                  inode->start_block + inode_used_size(*inode), false);
  }
  memset(inode, 0, sizeof(Inode));
  write_superblock();
}

void recursive_delete_inode(Inode *inode, uint8_t index) {

  char name[5];
  strncpy(name, inode->name, 5);
  // printf("DELETING DIRECTORY INODE %s with index: %u\n", name, index);
  for (int i = 0; i < 126; i += 1) {
    if (i == index)
      continue;
    Inode *other_inode = &SUPER_BLOCK->inode[i];
    if (inode_is_free(*other_inode))
      continue;

    if (inode_parent(*other_inode) != index)
      continue;

    // for each inode whose parent is the index
    if (inode_is_file(*other_inode)) {
      delete_inode(other_inode);
      continue;
    }

    if (inode_is_directory(*other_inode)) {
      recursive_delete_inode(other_inode, i);
      delete_inode(other_inode);
      continue;
    }
  }

  delete_inode(inode);
}

void print_file(char name[5], uint8_t size) {
  printf("%-5.5s %3d KB\n", name, size);
}

void print_directory(char name[5], uint8_t num_children) {
  printf("%-5s %3d\n", name, num_children + 2); // add 2 for . and ..
}

uint8_t num_children(uint8_t index) {
  uint8_t count = 0;
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];

    if (inode_is_free(inode))
      continue;

    if (inode_parent(inode) == index) {
      count += 1;
      continue;
    }
  }
  return count;
}

bool inode_name_equals(Inode inode, char name[5]) {
  return strncmp(inode.name, name, 5) == 0;
}

bool inode_name_not_equals(Inode inode, char name[5]) {
  return !inode_name_equals(inode, name);
}

uint8_t get_inode_with_name_in_cwd(char name[5]) {
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode *inode = &SUPER_BLOCK->inode[i];
    if (inode_is_free(*inode))
      continue;

    if (inode_parent(*inode) != CWD)
      continue;

    if (inode_name_equals(*inode, name))
      return i;
  }
  return 255;
}

bool can_allocate_start_block(uint8_t start, uint8_t size) {

  for (uint8_t i = start; i < start + size; i += 1) {
    if (block_in_use(i)) {
      return false;
    }
  }
  return true;
}

uint8_t get_start_block_for_allocation(uint8_t size, uint8_t search_start) {

  for (uint8_t candidate = search_start; candidate < 128; candidate += 1) {
    bool candidate_works = can_allocate_start_block(candidate, size);

    if (candidate_works) {
      return candidate;
    }
  }
  return 255;
}

// assumes block number is valid
void read_into_buffer(int block, uint8_t buffer[1024]) {
  //if (lseek(DISK_FD, block * 1024, SEEK_SET) < -1) {
  //  perror("error seeking to the block");
  //  return;
  //}
  if (pread(DISK_FD, buffer, 1024, block * 1024) < 0) {
    perror("error reading block to buffer");
    return;
  }
}

void write_buffer(int block, uint8_t buffer[1024]) {
  // printf("WRITING TO BLOCK %d\n", block);

  //if (lseek(DISK_FD, block * 1024, SEEK_SET) < -1) {
  //  perror("error seeking to the block");
  //  return;
  //}
  if (pwrite(DISK_FD, buffer, 1024, block * 1024) < 0) {
    perror("error writing block to buffer");
    return;
  }
}

void copy_block(uint8_t src, uint8_t dest) {
  uint8_t buffer[1024] = {0};
  read_into_buffer(src, buffer);
  write_buffer(dest, buffer);
}

void erase_block(uint8_t block) {
  uint8_t buffer[1024] = {0};
  write_buffer(block, buffer);
}

int inode_compare(const Inode *i1, const Inode *i2) {
  if (i1->start_block < i2->start_block)
    return -1;

  if (i1->start_block > i2->start_block)
    return 1;

  return 0;
}
