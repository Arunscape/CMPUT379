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
  //
  for (size_t i=0; i<126; i+=1){
    Inode node = SUPER_BLOCK->inode[i];
    for (uint8_t index; index < node.used_size; index += 1){
      // for each inode, if size is nonzero
      if (
    }
  }
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
