#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FileSystem.h"
#include "consistency_checks.h"

// globals
Super_block* SUPER_BLOCK = NULL;
FILE* FS;

void fs_mount(char *new_disk_name){
  
  // check if a virtual disk exists with the given name in the current directory
  // if not, 
  FS = fopen(new_disk_name, "rb+");
  if (FS == NULL) {
    perror("delete me");
    fprintf(stderr, "Error: Cannot find disk %s", new_disk_name);
    return;
  }

  // load the superblock
//   SUPER_BLOCK = (Super_block*) malloc(sizeof (Super_block));
 // memcpy(SUPER_BLOCK, FS, 1024);
  SUPER_BLOCK = (Super_block*) FS;

  
  // check for consistency of filesystem
  bool checks = do_checks();
  if (!checks) {
    // use the last filesystem that was mounted
    //
    //
    //else if no fs was mounted successfully before
    fprintf(stderr, "Error: No file system is mounted\n");
    return;
  } 


  // set cwd to /
}

void fs_create(char name[5], int size){
  if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
    fprintf(stderr, "Error: Superblock in <disk name> is full, cannot create %s", name);

  for (size_t i=0; i<126; i+=1){
    if (strcmp(name, SUPER_BLOCK->inode[i].name) == 0){
      fprintf(stderr, "Error: File or directory %s already exists", name);
    }
  }

  // not enough space
  fprintf(stderr, "Cannot allocate %d, on <disk name>", size);
}
void fs_delete(char name[5]){}
void fs_read(char name[5], int block_num){}
void fs_write(char name[5], int block_num){}
void fs_buff(uint8_t buff[1024]){}
void fs_ls(void){}
void fs_resize(char name[5], int new_size){}
void fs_defrag(void){}
void fs_cd(char name[5]){}
