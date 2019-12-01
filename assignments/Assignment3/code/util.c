#define _GNU_SOURCE
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FileSystem.h"
#include "consistency_checks.h"

extern int DISK_FD;
extern int CWD;
extern Super_block *SUPER_BLOCK;

bool attempt_mount(char *new_disk_name) {
  // check if a virtual disk exists with the given name in the current directory
  // if not,
  DISK_FD = open(new_disk_name, O_RDWR);
  if (DISK_FD < 0) {
    perror("delete me");
    fprintf(stderr, "Error: Cannot find disk %s\n", new_disk_name);
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
    fprintf(stderr,
            "Error: File system in %s is inconsistent (error code: %i)\n",
            new_disk_name, error_code);
    // use the last filesystem that was mounted

    return false;
  }

  // set cwd to /
  CWD = 127;

  return true;
}

uint8_t inode_parent(Inode inode) { return inode.dir_parent & 0b01111111; }

bool inode_in_use(Inode inode) { return (inode.used_size >> 7) & 1; }
bool inode_is_free(Inode inode) { return !inode_in_use(inode); }

bool inode_is_directory(Inode inode) { return (inode.dir_parent >> 7) & 1; }
bool inode_is_file(Inode inode) { return !inode_is_directory(inode); }

void write_superblock() {
  if (write(DISK_FD, SUPER_BLOCK, 1024) < 0) {
    fprintf(stderr, "WHAT");
  }
}
