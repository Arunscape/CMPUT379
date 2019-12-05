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
uint8_t BUFFER[1024] = {0};

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

    // printf("create: NO INODES AVAILABLE\n"); // TODO

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
    // printf("create: NO BLOCKS AVAILABLE\n"); // TODO
    fprintf(stderr, "Error: Cannot allocate %d, on <TODO GET DISK NAME>", size);
    return;
  }

  if (size > 0) { // file

    uint8_t start_block =
        get_start_block_for_allocation(size, first_available_block);

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

    if (inode_name_not_equals(*inode, name))
      continue;

    if (inode_is_file(*inode)) {
      delete_inode(inode);
      did_not_delete = false;
      break;
    }
    if (inode_is_directory(*inode)) {
      recursive_delete_inode(inode, i);
      did_not_delete = false;
      break;
    }
  }

  if (did_not_delete)
    fprintf(stderr, "File or directory %s does not exist\n", name);
}

void fs_read(char name[5], int block_num) {
  // printf("\n\nREADING FILE WITH NAME %s, and block num %d\n\n", name,
  //     block_num);

  bool did_not_read_into_buffer = true;
  bool invalid_block_num = false;
  // find the file with the name in the current directory
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];

    if (inode_not_in_cwd(inode))
      continue;

    if (inode_is_directory(inode))
      continue;

    if (inode_name_not_equals(inode, name))
      continue;

    // we're in the same directory, and the name matches, and it's a file
    if (block_num < 0 || block_num > (inode_used_size(inode) - 1)) {
      invalid_block_num = true;
      break;
    }

    // also the block num is valid now

    read_into_buffer(inode.start_block + block_num, BUFFER);

    did_not_read_into_buffer = false;
  }

  if (did_not_read_into_buffer) {
    fprintf(stderr, "Error: file %s does not exist", name);
    return;
  }

  if (invalid_block_num) {
    fprintf(stderr, "Error, %s does not have block %u\n", name, block_num);
    return;
  }
}

void fs_write(char name[5], int block_num) {
  bool did_not_write_buffer = true;
  bool invalid_block_num = false;
  // find the file with the name in the current directory
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];

    if (inode_not_in_cwd(inode))
      continue;

    if (inode_is_directory(inode))
      continue;

    if (inode_name_not_equals(inode, name))
      continue;

    // we're in the same directory, and the name matches, and it's a file
    if (block_num < 0 || block_num > (inode_used_size(inode) - 1)) {
      invalid_block_num = true;
      break;
    }

    // also the block num is valid now
    printf("INODE START BLOCK: %d\n", inode.start_block);
    printf("INODE NAME: %s\n", inode.name);
    write_buffer(inode.start_block + block_num, BUFFER);
    did_not_write_buffer = false;
  }

  if (did_not_write_buffer) {
    fprintf(stderr, "Error: file %s does not exist", name);
    return;
  }

  if (invalid_block_num) {
    fprintf(stderr, "Error, %s does not have block %u\n", name, block_num);
    return;
  }
}

void fs_buff(uint8_t buff[1024]) {
  for (size_t i = 0; i < 1024; i += 1)
    BUFFER[i] = buff[i];
}

void fs_ls(void) {

  uint8_t cwd_count = num_children(CWD);
  print_directory(".", cwd_count);

  uint8_t parent_count = num_children(inode_parent(SUPER_BLOCK->inode[CWD]));
  print_directory("..", parent_count);

  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];

    if (inode_is_free(inode))
      continue;

    if (inode_not_in_cwd(inode))
      continue;

    if (inode_is_file(inode)) {
      print_file(inode.name, inode_used_size(inode));
      continue;
    }

    if (inode_is_directory(inode)) {
      uint8_t count = num_children(i);
      print_directory(inode.name, count);
      continue;
    }
  }
}

void fs_resize(char name[5], int new_size) {
  uint8_t inode_index = get_inode_with_name_in_cwd(name);

  Inode *inode = &SUPER_BLOCK->inode[inode_index];

  if (inode == NULL || inode_is_directory(*inode)) {
    fprintf(stderr, "Error: File %s does not exist\n", name);
    return;
  }

  bool error = false;
  if (new_size > inode_used_size(*inode)) {
    // need to allocate more blocks to this file
    if (can_allocate_start_block(inode->start_block, new_size)) {
      update_blocks(inode->start_block, inode->start_block + new_size, false);
      update_inode(inode_index, inode->name, new_size, inode->start_block,
                   inode->dir_parent, true, false);
      return;
    }

    uint8_t start_block = get_start_block_for_allocation(new_size, 0);
    if (start_block > 127) {
      error = true;
    } else {

      // copy the blocks over

      for (uint8_t i = 0; i < inode_used_size(*inode); i += 1) {
        copy_block(inode->start_block + i, start_block + i);
      }
      update_blocks(start_block, start_block + new_size, false);
      update_inode(inode_index, inode->name, new_size, start_block,
                   inode->dir_parent, true, false);
      return;
    }
  }

  if (error) {
    fprintf(stderr, "Error: File %s cannot expand to size %d\n", inode->name,
            new_size);
    return;
  }

  if (new_size < inode_used_size(*inode)) {
    uint8_t zero_start = inode->start_block + new_size;
    uint8_t zero_end = inode->start_block + inode_used_size(*inode);
    update_blocks(zero_start, zero_end, false);
    update_inode(inode_index, inode->name, new_size, inode->start_block,
                 inode->dir_parent, true, false);
    return;
  }

  // new size equals old size
}

void fs_defrag(void) {

  Inode *inodes[126] = {0};
  uint8_t count = 0;
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode *inode = &SUPER_BLOCK->inode[i];
    if (inode_is_free(*inode))
      continue;
    inodes[count] = inode;
    count += 1;
  }

  qsort(&inodes, count, sizeof(Inode *), inode_compare);

  uint8_t start_block = 1;
  for (uint8_t i = 0; i < count; i += 1) { // for each file
    Inode *inode = inodes[i];

    for (uint8_t offset = 0; offset < inode_used_size(*inode); offset += 1) {
      copy_block(inode->start_block + offset, start_block + offset);
    }
    update_blocks(inode->start_block,
                  inode->start_block + inode_used_size(*inode), false);
    update_blocks(start_block, start_block + inode_used_size(*inode), true);
    inode->start_block = start_block;
    start_block += inode_used_size(*inode);
  }
  write_superblock();
}

void fs_cd(char name[5]) {

  if (strcmp(name, ".") == 0)
    return;

  if (strcmp(name, "..") == 0) {
    if (CWD == 127)
      return;
    Inode inode = SUPER_BLOCK->inode[CWD]; // should be a directory
    CWD = inode_parent(inode);
  }

  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];
    if (inode_is_free(inode))
      continue;

    if (inode_is_file(inode))
      continue;

    if (inode_parent(inode) != CWD)
      continue;

    if (inode_name_not_equals(inode, name))
      continue;

    CWD = i;
    return;
  }

  fprintf(stderr, "Error: Directory %s does not exist\n", name);
}
