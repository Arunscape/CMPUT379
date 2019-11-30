#include <stdbool.h>
#include <string.h>

#include "FileSystem.h"
#include "util.h"

extern Super_block *SUPER_BLOCK;
extern FILE *FS;

bool check_one() {
  // check between free_block_list and inodes
  // https://eclass.srv.ualberta.ca/mod/forum/discuss.php?d=1260785

  // for each bit in the free block list, loop over the Inodes and check
  // if it's in use
  // direct quote from TA: efficiency is not a concern here

  // assert superblock is marked in use in free space list
  if (((SUPER_BLOCK->free_block_list[0] >> 7) & 1) != 1) {
    return false;
  }

  for (uint8_t i = 1; i < 128; i += 1) {
    uint8_t index = i / 8;
    uint8_t shift = 7 - (i % 8);

    // block is in use
    uint8_t in_use = (SUPER_BLOCK->free_block_list[index] >> shift) & 1;
    // printf("in use: %d", in_use);
    uint8_t usage_count = 0;
    for (uint8_t j = 0; j < 126; j += 1) {
      Inode inode = SUPER_BLOCK->inode[i];

      // if inode is a file and marked in use
      // bool inode_is_file = (inode.dir_parent >> 7) & 1;
      // bool inode_is_in_use = (inode.used_size >> 7) & 1;

      // printf("inode is file: %d, inode in use: %d, start_block: %d\n",
      // inode_is_file, inode_is_in_use, inode.start_block );

      if (inode_is_file(inode) && inode_in_use(inode)) {
        uint8_t start = inode.start_block;
        uint8_t end = inode.start_block + (inode.used_size & 0b01111111);

        if (start <= i && i < end)
          usage_count += 1;
      }

      // blocks marked in use in the free space list must be allocated to //
      // exactly one file
      if (in_use && usage_count != 1) {
        fprintf(stderr, "block %d was marked in use but usage count is %d\n", i,
                usage_count);
        return false;
      }

      // blocks that are marked free in the free-space list cannot be allocated
      // to any file
      if (!in_use && usage_count > 0) {
        fprintf(stderr, "block %d was marked free but usage count is %d\n", i,
                usage_count);
        return false;
      }
    }
  }

  return true;
}
bool check_two() {
  // name of every file/directory must be unique in each directory

  // for every inode that is a directory
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];

    if (inode_is_directory(inode)) {
      for (uint8_t j = 0; j < 126; j += 1) {
        if (i == j)
          continue;
        Inode other_inode = SUPER_BLOCK->inode[i];
        // if other inode is in the original inode's directory
        if ((other_inode.dir_parent & 0b01111111) == i) {
          Inode other_inode = SUPER_BLOCK->inode[i];

          if (strncmp(inode.name, other_inode.name, 5) == 0) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool check_three() {

  // if state of an inode is free, all bits in that inode must be zero
  //
  // else the name attribute stored in the inode must have at least one bit
  // that is not zero

  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];

    if (inode_is_free(inode)) {
      if (inode.name[0] || inode.name[1] || inode.name[2] || inode.name[3] ||
          inode.name[4] || inode.used_size || inode.start_block ||
          inode.dir_parent)
        return false;
    } else {
      if (inode.name[0] || inode.name[1] || inode.name[2] || inode.name[3] ||
          inode.name[4])
        return false;
    }
  }
  return true;
}

bool check_four() {
  // the start block of every inode that is marked as a file must have a value
  // between 1 and 127 inclusive

  // TODO check https://eclass.srv.ualberta.ca/mod/forum/discuss.php?d=1269743
  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];
    if (inode_is_file(inode) && inode_in_use(inode))
      if (!(1 <= inode.start_block && inode.start_block <= 127))
        return false;
  }
  return true;
}

bool check_five() {
  // the size and start_block of an inode that is marked as a directory must be
  // zero

  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];
    if (inode_is_directory(inode) && inode_in_use(inode))
      if (inode.used_size || inode.start_block)
        return false;
  }
  return true;
}

bool check_six() {
  // for every inode, the index of its parent inode cannot be 126
  // also, if the index of the parent inode is between 0 and 125 inclusive,
  // then the parent inode must be in use and marked as a directory

  for (uint8_t i = 0; i < 126; i += 1) {
    Inode inode = SUPER_BLOCK->inode[i];
    if (!inode_in_use(inode))
      continue;
    uint8_t parent_index = inode.dir_parent & 0b01111111;
    if (parent_index == 126)
      return false;

    if (0 <= parent_index && parent_index <= 125)
      if (!inode_in_use(inode) || !inode_is_directory(inode))
        return false;
  }
  return true;
}

int8_t do_checks() {

  int error_code = 0;

  if (!check_one())
    error_code = 1;
  if (!check_two())
    error_code = 2;
  if (!check_three())
    error_code = 3;
  if (!check_four())
    error_code = 4;
  if (!check_five())
    error_code = 5;
  if (!check_six())
    error_code = 6;

  return error_code;
}
