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
void print_file(char name[5], uint8_t size);

// void print_directory(char name[5], uint8_t num_children);

void calculate_and_print_directory(Inode inode, uint8_t index);

bool inode_not_in_cwd(Inode inode);
bool inode_in_cwd(Inode inode);

bool inode_name_equals(Inode inode, char name[5]);

bool inode_name_not_equals(Inode inode, char name[5]);
uint8_t inode_used_size(Inode inode);

Inode *get_inode_with_name_in_cwd(char name[5]);
uint8_t get_start_block_for_allocation(uint8_t size, uint8_t search_start);
#endif
