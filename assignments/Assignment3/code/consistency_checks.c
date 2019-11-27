#include <stdbool.h>

#include "FileSystem.h"

extern Super_block* SUPER_BLOCK;
extern FILE* FS;

bool check_one(){
  //
  //

  // check between free_block_list and inodes
  // https://eclass.srv.ualberta.ca/mod/forum/discuss.php?d=1260785
  //
  
  // for each bit in the free block list, loop over the Inodes and check
  // if it's in use
  // direct quote from TA: efficiency is not a concern here
  
  // todo assert block 0 is 1 (in use)
  //
  for (uint8_t i=1; i< 128; i+=1){
    uint8_t index = i / 8;
    uint8_t shift = 7 - (i % 8);

    // block is in use
    uint8_t in_use = (SUPER_BLOCK->free_block_list[index] >> shift) & 1;
    printf("in use: %d", in_use);
    uint8_t usage_count = 0;
    for (uint8_t j=0; j< 126; j+=1){
      Inode inode = SUPER_BLOCK->inode[i];
      
      // if inode is a file and marked in use
      bool inode_is_file = (inode.dir_parent >> 7) & 1;
      bool inode_is_in_use = (inode.used_size >> 7) & 1;

//      printf("inode is file: %d, inode in use: %d, start_block: %d\n",
//          inode_is_file, inode_is_in_use, inode.start_block );
      if ( inode_is_file && inode_is_in_use ){ 
        uint8_t start = inode.start_block;
        uint8_t end = inode.start_block + (inode.used_size & 0b01111111);

        if ( start <= i && i < end)
          usage_count += 1;
      }


      // blocks marked in use in the free space list must be allocated to             // exactly one file
      if (in_use && usage_count != 1){
        fprintf(stderr, "block %d was marked in use but usage count is %d\n", i, usage_count);
        return false;
      }

      // blocks that are marked free in the free-space list cannot be allocated 
      // to any file
      if (!in_use && usage_count > 0){
        fprintf(stderr, "block %d was marked free but usage count is %d\n", i, usage_count);
        return false;
      }
    }
  }

  return true;
}
bool check_two(){
  // name of every file/directory must be unique in each directory
  return true;
}

bool check_three(){
  
  // if state of an inode is free, all bits in that inode must be zero
  //
  // else the name attribute stored in the inode must have at least one bit 
  // that is not zero
  return true;
}

bool check_four(){
  // the start block of every inode that is marked as a file must have a value
  // between 1 and 127 inclusive
  return true;
}

bool check_five(){
  // the size and start_block of an inode that is marked as a directory must be
  // zero
  return true;
}

bool check_six(){
  // for every inode, the index of its parent inode cannot be 126
  // also, if the index of the parent inode is between 0 and 125 inclusive,
  // then the parent inode must be in use and marked as a directory
  return true;
}


bool do_checks(){
  return check_one() && 
         check_two() && 
         check_three() && 
         check_four() &&
         check_five() &&
         check_six();
}
