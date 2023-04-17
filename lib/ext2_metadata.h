#ifndef _EXT2_METADATA_H_
#define _EXT2_METADATA_H_

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define EXT2_MAGIC_NUMBER 0xEF53

#define SUPERBLOCK_OFFSET 1024

#define BLOCKS_COUNT_OFFSET 4
#define R_BLOCKS_COUNT_OFFSET 8
#define FREE_BLOCKS_COUNT_OFFSET 12
#define FREE_INODES_COUNT_OFFSET 16
#define FIRST_DATA_BLOCK_OFFSET 20
#define LOG_BLOCK_SIZE_OFFSET 24
#define BLOCKS_PER_GROUP_OFFSET 32
#define FRAGS_PER_GROUP_OFFSET 36
#define INODES_PER_GROUP_OFFSET 40
#define MTIME_OFFSET 44
#define WTIME_OFFSET 48

#define S_MAGIC_OFFSET 56

#define LASTCHECK_OFFSET 64

#define FIRST_INO_OFFSET 84
#define INODE_SIZE_OFFSET 88

#define VOLUME_NAME_OFFSET 120

/**
 * Struct for ext2 metadata (not all fields are implemented)
 * @ref: https://www.nongnu.org/ext2-doc/ext2.html
*/
typedef struct {

    /**
     * Inode info
    */
    int s_inodes_count;
    short int s_inode_size;
    int s_first_ino;
    int s_inodes_per_group;
    int s_free_inodes_count;
    
    /**
     * Info block
    */
    int s_log_block_size; // block size = 1024 << s_log_block_size
    int s_r_blocks_count;
    int s_free_blocks_count;
    int s_blocks_count; // total blocks count
    int s_first_data_block;
    int s_blocks_per_group;
    int s_frags_per_group;

    /**
     * Info volume
    */
    char s_volume_name[16];
    int s_lastcheck; // last check time
    int s_mtime; // last mount/edited time 
    int s_wtime; // last write time

    // s_magic identify the filesystem as Ext2
} EXT2_metadata;

int EXT2_METADATA_isEXT2(int fd);

void EXT2_METADATA_lseek_or_die(int fd, int offset, int action);

void EXT2_METADATA_read_or_die(int fd, void *buffer, int size, char *error_msg);

EXT2_metadata EXT2_METADATA_init(int fd);

void EXT2_METADATA_print(EXT2_metadata metadata);

#endif