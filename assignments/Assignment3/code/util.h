#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdint.h>
#include "FileSystem.h"

bool attempt_mount(char* new_disk_name);

uint8_t inode_parent(Inode inode);

bool inode_in_use(Inode inode);
bool inode_is_free(Inode inode);

bool inode_is_directory(Inode inode);
bool inode_is_file(Inode inode);
#endif
