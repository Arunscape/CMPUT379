#include <stdbool.h>

#include "FileSystem.h"

extern Super_block* SUPER_BLOCK;
extern FILE* FS;

bool check_one(){
  // blocks that are marked free in the free-space list cannot be allocated 
  // to any file
  //
  // blocks marked in use in the free space list must be allocated to exactly
  // one file
  //

  // check between free_block_list and inodes
  // https://eclass.srv.ualberta.ca/mod/forum/discuss.php?d=1260785
  //
  for(size_t i=2; i<=128; i+=1){
    uint8_t bit = SUPER_BLOCK->free_block_list[i/8] >> (i % 8) & 0b00000001;

    if (i == 1 && bit == 0){
      fprintf(stderr, "first block is the superblock, that can't be free\n");
      return false;
    }

    if(bit && SUPER_BLOCK->inode[i-2].name[0] != NULL){
        fprintf(stderr, "UHHHHHH");
        return false;
    }
    else if (!bit && SUPER_BLOCK->inode[i-2].name[0] == NULL){
        fprintf(stderr, "HMMMMMMM");
        return false;
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
