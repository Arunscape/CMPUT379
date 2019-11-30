#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>

#include "FileSystem.h"
#include "consistency_checks.h"

// globals
Super_block *SUPER_BLOCK = NULL;

int DISK_FD = -1;
char* CWD = NULL;

bool attempt_mount(char* new_disk_name){
    // check if a virtual disk exists with the given name in the current directory
    // if not,
    DISK_FD = open(new_disk_name, O_RDWR);
    if (DISK_FD < 0) {
        perror("delete me");
        fprintf(stderr, "Error: Cannot find disk %s", new_disk_name);
        return false;
    }

    SUPER_BLOCK = malloc(sizeof(Super_block)); // TODO free
    if (read(DISK_FD, SUPER_BLOCK, 1024) < 0) {
        perror("uhhh");
        fprintf(stderr, "ERROR REEADING SUPER BLOCK\n");
    }

    // check for consistency of filesystem
    int8_t error_code = do_checks();
    if (error_code != 0) {
        fprintf(stderr, "Error: File system in %s is inconsistent (error code: %i)\n", new_disk_name, error_code);
        // use the last filesystem that was mounted

        return false;
    }

    // set cwd to /
    CWD = NULL;
    if (asprintf(&CWD, "/") < 0){
        perror("asprintf failed");
        return false;
    }

    return true;
}

void fs_mount(char *new_disk_name) {
    if (attempt_mount(new_disk_name)){
        // successfully mounted
        return;
    }

    // unsuccessful mount, see if filesystem was mounted before
    if (DISK_FD == -1)
        fprintf(stderr, "Error: No file system is mounted\n");
}

void fs_create(char name[5], int size) {
  if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0){
    // TODO ask TA what the error message should be
    fprintf(stderr, ". and .. are reserved names\n");
    return;
  }

  // i don't know what the best way to store cwd is

  for (size_t i = 0; i < 126; i += 1) {
    if (strcmp(name, SUPER_BLOCK->inode[i].name) == 0) {
      fprintf(stderr, "Error: File or directory %s already exists", name);
    }
  }

    fprintf(stderr,
            "Error: Superblock in <disk name> is full, cannot create %s", name);
  // not enough space
  fprintf(stderr, "Cannot allocate %d, on <disk name>", size);
}
void fs_delete(char name[5]) {
  for (size_t i = 0; i < 126; i += 1) {
    if (strcmp(name, SUPER_BLOCK->inode[i].name) == 0) {
      // delete it
      return;
    }
  }
  fprintf(stderr, "File or directory %s does not exist", name);
}
void fs_read(char name[5], int block_num) {}
void fs_write(char name[5], int block_num) {}
void fs_buff(uint8_t buff[1024]) {}
void fs_ls(void) {}
void fs_resize(char name[5], int new_size) {}
void fs_defrag(void) {}
void fs_cd(char name[5]) {}
