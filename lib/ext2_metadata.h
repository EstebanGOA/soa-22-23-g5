#ifndef _EXT2_METADATA_H_
#define _EXT2_METADATA_H_

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define SUPERBLOCK_OFFSET 1024
#define INODE_SIZE_OFFSET 88
#define INODE_FIRST_INO_OFFSET 84
#define INODES_PER_GROUP_OFFSET 40
#define FREE_INODES_COUNT_OFFSET 16

/**
 * Struct for ext2 metadata (not all fields are implemented)
 * @ref: https://www.nongnu.org/ext2-doc/ext2.html
*/
typedef struct {
    int s_inodes_count;
    short int s_inode_size;
    int s_first_ino;
    int s_inodes_per_group;
    int s_free_inodes_count;
} EXT2_metadata;


void EXT2_METADATA_init(EXT2_metadata *metadata, char *path);

void EXT2_METADATA_print(EXT2_metadata *metadata);

#endif