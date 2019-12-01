#ifndef UTIL_H
#define UTIL_H

#include "FileSystem.h"
#include <stdbool.h>
#include <stdint.h>

bool attempt_mount(char *new_disk_name);

uint8_t inode_parent(Inode inode);

bool inode_in_use(Inode inode);
bool inode_is_free(Inode inode);

bool inode_is_directory(Inode inode);
bool inode_is_file(Inode inode);

void write_superblock();

bool block_is_free(int8_t i);

int8_t get_first_available_block();
int8_t get_first_available_inode();

bool is_a_duplicate(char* name);
#endif
