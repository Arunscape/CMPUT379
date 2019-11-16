#include <stdbool.h>



bool check_one(){
  // blocks that are marked free in the free-space list cannot be allocated 
  // to any file
  //
  // blocks marked in use in the free space list must be allocated to exactly
  // one file
}

bool check_two(){
  // name of every file/directory must be unique in each directory
}

bool check_three(){
  
  // if state of an inode is free, all bits in that inode must be zero
  //
  // else the name attribute stored in the inode must have at least one bit 
  // that is not zero
}

bool check_four(){
  // the start block of every inode that is marked as a file must have a value
  // between 1 and 127 inclusive
}

bool check_five(){
  // the size and start_block of an inode that is marked as a directory must be
  // zero
}

bool check_six(){
  // for every inode, the index of its parent inode cannot be 126
  // also, if the index of the parent inode is between 0 and 125 inclusive,
  // then the parent inode must be in use and marked as a directory
}


bool do_checks(){
  return check_one() && 
         check_two() && 
         check_three() && 
         check_four() &&
         check_five() &&
         check_six();
}
