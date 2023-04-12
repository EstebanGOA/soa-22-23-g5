#ifndef _EXT2_METADATA_H_
#define _EXT2_METADATA_H_

/**
 * Struct for ext2 metadata (not all fields are implemented)
 * @ref: https://www.nongnu.org/ext2-doc/ext2.html
*/
struct ext2 {
    int s_inodes_count;
    int s_blocks_count;
    int s_r_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    int s_first_data_block;
    int s_log_block_size;
    int s_log_frag_size;
    int s_blocks_per_group;
    int s_frags_per_group;
    int s_inodes_per_group;
    int s_mtime;
    int s_wtime;
    int s_mnt_count;
    short int s_max_mnt_count;
    short int s_magic;
    short int s_state;
    short int s_errors;
    short int s_minor_rev_level;
    int s_lastcheck;
    int s_checkinterval;
    int s_creator_os;
    int s_rev_level;
    short int s_def_resuid;
    short int s_def_resgid;
    int s_first_ino;
    short int s_inode_size;
    short int s_block_group_nr;
    int s_feature_compat;
    int s_feature_incompat;
    int s_feature_ro_compat;
    char s_uuid[16];
    char s_volume_name[16];
}

#define SUPERBLOCK_OFFSET 1024;

void EXT2_METADATA_init(struct ext2 *metadata, char *path);
void EXT2_METADATA_print(struct ext2 *metadata);

#endif