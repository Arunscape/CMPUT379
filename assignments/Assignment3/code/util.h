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
bool block_in_use(int8_t i);

int8_t get_first_available_block();
int8_t get_first_available_inode();

bool is_a_duplicate(char *name);

void update_inode(int8_t i, char name[5], uint8_t size, uint8_t start_block,
                  uint8_t dir_parent, bool used, bool is_directory);

void update_blocks(uint8_t start, uint8_t end, bool set);

bool no_filesystem_mounted();

void delete_inode(Inode *inode);
void recursive_delete_inode(Inode *inode, uint8_t index);

void seek_beginning_file();
#endif
