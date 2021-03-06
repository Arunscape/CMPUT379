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
    // fprintf(stderr, "check 1: block 0 isn't marked as used\n");
    return false;
  }

  for (uint8_t i = 1; i < 128; i += 1) {
    // printf("in use: %d", in_use);
    uint8_t usage_count = 0;
    for (uint8_t j = 0; j < 126; j += 1) {
      Inode inode = SUPER_BLOCK->inode[j];

      // if inode is a file and marked in use
      // bool inode_is_file = (inode.dir_parent >> 7) & 1;
      // bool inode_is_in_use = (inode.used_size >> 7) & 1;

      // printf("inode is file: %d, inode in use: %d, start_block: %d\n",
      // inode_is_file, inode_is_in_use, inode.start_block );

      if (inode_is_file(inode) && inode_in_use(inode)) {
        uint8_t start = inode.start_block;
        uint8_t end = inode.start_block + inode_used_size(inode);

        // printf(
        //  "inspecting inode %d, name: %s, size: %u, start: %u,parent: %u\n",
        //  j, inode.name, inode.used_size, inode.start_block,
        //  inode.dir_parent);
        // printf("start: %u, block: %u, end: %u\n", start, i, end);
        if (start <= i && i < end)
          usage_count += 1;
      }
    }

    // blocks marked in use in the free space list must be allocated to //
    // exactly one file
    if (block_in_use(i) && usage_count != 1) {
      // fprintf(stderr,
      //        "check1: block %u was marked in use but usage count is %u\n", i,
      //        usage_count);
      return false;
    }

    // blocks that are marked free in the free-space list cannot be allocated
    // to any file
    if (block_is_free(i) && usage_count > 0) {
      // fprintf(stderr,
      //        "check1: block %d was marked free but usage count is %d\n", i,
      //        usage_count);
      return false;
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
        if (inode_parent(other_inode) == i) {
          Inode other_inode = SUPER_BLOCK->inode[i];

          if (inode_name_equals(inode, other_inode.name)) {
            // fprintf(stderr, "check2: non unique filename/directory\n");
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
          inode.dir_parent) {
        // fprintf(stderr, "inode %d is marked free, but its not zeroed out\n",
        // i);
        return false;
      }
    } else {
      if ((inode.name[0] || inode.name[1] || inode.name[2] || inode.name[3] ||
           inode.name[4]) == 0) {
        // fprintf(stderr, "inode %d is marked used, but its name is zeroed
        // out\n",
        //        i);
        return false;
      }
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
      if (inode_used_size(inode) || inode.start_block)
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
    if (inode_is_free(inode))
      continue;
    if (inode_parent(inode) == 126)
      return false;

    uint8_t parent_index = inode_parent(inode);
    if (0 <= parent_index && parent_index <= 125) {
      Inode parent = SUPER_BLOCK->inode[parent_index];
      if (inode_is_free(parent) || inode_is_file(parent))
        return false;
    }
  }
  return true;
}

int8_t do_checks() {

  if (!check_one())
    return 1;
  if (!check_two())
    return 2;
  if (!check_three())
    return 3;
  if (!check_four())
    return 4;
  if (!check_five())
    return 5;
  if (!check_six())
    return 6;

  return 0;
}
