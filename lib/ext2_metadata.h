#ifndef _EXT2_MODULE_H_
#define _EXT2_MODULE_H_

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

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

#define EXT2_S_IFDIR 0x4000
#define EXT2_S_IFREG 0x8000
#define EXT2_FT_DIR 2
#define EXT2_FT_REG_FILE 1

/**
 * Struct for ext2 metadata (not all fields are implemented)
 * @ref: https://www.nongnu.org/ext2-doc/ext2.html#superblock
*/
typedef struct {

    /**
     * Inode info
    */
    uint32_t s_inodes_count;
    uint16_t s_inode_size;
    uint32_t s_first_ino;
    uint32_t s_inodes_per_group;
    uint32_t s_free_inodes_count;
    
    /**
     * Info block
    */
    uint32_t s_log_block_size; // block size = 1024 << s_log_block_size
    uint32_t s_r_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_blocks_count; // total blocks count
    uint32_t s_first_data_block;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;

    /**
     * Info volume
    */
    char s_volume_name[16];
    uint32_t s_lastcheck; // last check time
    uint32_t s_mtime; // last mount/edited time 
    uint32_t s_wtime; // last write time

    // s_magic identify the filesystem as Ext2
} EXT2_metadata;

/**
 * Block Group Descriptor Table
 * #@ref: https://www.nongnu.org/ext2-doc/ext2.html#block-group-descriptor-table
*/
typedef struct{
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    char bg_reserved[12];
} EXT2_group;

/**
 * Inode table
 * @ref: https://www.nongnu.org/ext2-doc/ext2.html#inode-table
*/
typedef struct {
    uint16_t i_mode;
    uint16_t i_uid;
    int32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    unsigned char i_osd2[12];
} EXT2_inode;

typedef struct {
    uint32_t inode;
    uint16_t rec_len;
    char name_len;
    char file_type;
    char *name;
} EXT2_entry;

int EXT2_MODULE_isEXT2(int fd);

void EXT2_MODULE_lseek_or_die(int fd, int offset, int action);

void EXT2_MODULE_read_or_die(int fd, void *buffer, int size, char *error_msg);

EXT2_metadata EXT2_MODULE_get_metadata(int fd);

void EXT2_MODULE_printMetadata(EXT2_metadata metadata);

EXT2_group EXT2_MODULE_get_group_descriptors(int fd, int block_size, int offset);

EXT2_inode EXT2_MODULE_get_inode(int fd, int position, EXT2_group group, EXT2_metadata metadata);

void EXT2_MODULE_read_inode(int fd, EXT2_inode inode, EXT2_metadata metadata, int level);

void EXT2_MODULE_show_filesystem(int fd);

void EXT2_MODULE_print_inode(EXT2_inode inode);

void EXT2_MODULE_print_group(EXT2_group);

void EXT2_MODULE_print(EXT2_inode inode);

#endif