#include <stdint.h>
#include <stdio.h>

#include "consistency_checks.h"


void fs_mount(char *new_disk_name){
  
  // check if a virtual disk exists with the given name in the current directory
  // if not, 
  fprintf(stderr, "Error: Cannot find disk %s", new_disk_name);
  return;

  // load the superblock
  
  // check for consistency of filesystem
  bool checks = do_checks();
  if (!checks) {
    // use the last filesystem that was mounted
    //
    //
    //else if no fs was mounted successfully before
    fprintf(stderr, "Error: No file system is mounted");
    return;
  } 
}

void fs_create(char name[5], int size){}
void fs_delete(char name[5]){}
void fs_read(char name[5], int block_num){}
void fs_write(char name[5], int block_num){}
void fs_buff(uint8_t buff[1024]){}
void fs_ls(void){}
void fs_resize(char name[5], int new_size){}
void fs_defrag(void){}
void fs_cd(char name[5]){}
