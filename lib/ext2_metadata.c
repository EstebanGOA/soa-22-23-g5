#include "ext2_metadata.h"

int EXT2_METADATA_isEXT2(int fd) {
    unsigned short int magic_number;
    lseek(fd, SUPERBLOCK_OFFSET + S_MAGIC_OFFSET, SEEK_SET);
    if (read(fd, &magic_number, sizeof(unsigned short int)) < 0) {
        printf("Error reading file");
        exit(1);
    }
    return magic_number == EXT2_MAGIC_NUMBER;
}

void EXT2_METADATA_lseek_or_die(int fd, int offset, int action)
{
    if(lseek(fd, SUPERBLOCK_OFFSET + offset, action) < 0)
    {
        printf("Error: could not seek to superblock");
        exit(1);
    }
}

void EXT2_METADATA_read_or_die(int fd, void *buffer, int size, char *error_msg) 
{
    int bytes_read = read(fd, buffer, size);
    if (bytes_read < 0)
    {
        printf("%s\n", error_msg);
        exit(1);
    }
}

EXT2_metadata EXT2_METADATA_init(int fd)
{
    EXT2_metadata metadata;
        
    EXT2_METADATA_lseek_or_die(fd, 0, SEEK_SET);
    EXT2_METADATA_read_or_die(fd, &metadata.s_inodes_count, sizeof(int), "Error: could not read inodes count");
    EXT2_METADATA_read_or_die(fd, &metadata.s_blocks_count, sizeof(int), "Error: could not read blocks count");
    EXT2_METADATA_read_or_die(fd, &metadata.s_r_blocks_count, sizeof(int), "Error: could not read reserved blocks count");
    EXT2_METADATA_read_or_die(fd, &metadata.s_free_blocks_count, sizeof(int), "Error: could not read free blocks count");
    EXT2_METADATA_read_or_die(fd, &metadata.s_free_inodes_count, sizeof(int), "Error: could not read free inodes count");
    EXT2_METADATA_read_or_die(fd, &metadata.s_first_data_block, sizeof(int), "Error: could not read first data block");
    EXT2_METADATA_read_or_die(fd, &metadata.s_log_block_size, sizeof(int), "Error: could not read log block size");

    EXT2_METADATA_lseek_or_die(fd, BLOCKS_PER_GROUP_OFFSET, SEEK_SET);
    EXT2_METADATA_read_or_die(fd, &metadata.s_blocks_per_group, sizeof(int), "Error: could not read blocks per group");
    EXT2_METADATA_read_or_die(fd, &metadata.s_frags_per_group, sizeof(int), "Error: could not read frags per group");
    EXT2_METADATA_read_or_die(fd, &metadata.s_inodes_per_group, sizeof(int), "Error: could not read inodes per group");
    EXT2_METADATA_read_or_die(fd, &metadata.s_mtime, sizeof(int), "Error: could not read mount time");
    EXT2_METADATA_read_or_die(fd, &metadata.s_wtime, sizeof(int), "Error: could not read write time");

    EXT2_METADATA_lseek_or_die(fd, LASTCHECK_OFFSET, SEEK_SET);
    EXT2_METADATA_read_or_die(fd, &metadata.s_lastcheck, sizeof(int), "Error: could not read last check");

    EXT2_METADATA_lseek_or_die(fd, FIRST_INO_OFFSET, SEEK_SET);
    EXT2_METADATA_read_or_die(fd, &metadata.s_first_ino, sizeof(int), "Error: could not read first inode");
    EXT2_METADATA_read_or_die(fd, &metadata.s_inode_size, sizeof(short int), "Error: could not read inode size");

    EXT2_METADATA_lseek_or_die(fd, VOLUME_NAME_OFFSET, SEEK_SET);
    EXT2_METADATA_read_or_die(fd, &metadata.s_volume_name, sizeof(char) * 16, "Error: could not read volume name");
    
    close(fd);

    return metadata;
}

void EXT2_METADATA_print(EXT2_metadata metadata)
{  
    char *buffer = NULL;
    time_t s_lastcheck = (time_t) metadata.s_lastcheck;
    time_t s_mtime = (time_t) metadata.s_mtime;
    time_t s_wtime = (time_t) metadata.s_wtime;
    
    printf("Filesystem: EXT2\n");
    printf("\nINODE INFO\n");
    printf("\tSize: %d\n", metadata.s_inode_size);
    printf("\tNum inodes: %d\n", metadata.s_inodes_count);
    printf("\tFirst inode: %d\n", metadata.s_first_ino);
    printf("\tInodes per group: %d\n", metadata.s_inodes_per_group);
    printf("\tFree inodes: %d\n", metadata.s_free_inodes_count);

    printf("\nINFO BLOCK\n");
    printf("\tBlock size: %d\n", (1024 << metadata.s_blocks_count));
    printf("\tReserved blocks: %d\n", metadata.s_r_blocks_count);
    printf("\tFree blocks: %d\n", metadata.s_free_blocks_count);
    printf("\tTotal blocks: %d\n", metadata.s_blocks_count);
    printf("\tFirst block: %d\n", metadata.s_first_data_block);
    printf("\tGroup blocks: %d\n", metadata.s_blocks_per_group);
    printf("\tGroup flags: %d\n", metadata.s_frags_per_group);

    
    printf("\nINFO VOLUME\n");
    printf("\tVolume name: %s\n", metadata.s_volume_name);

    buffer = (char *) malloc(sizeof(char) * 80); // 80 is the max length of a string
    strftime(buffer, sizeof(char) * 80, "%a %b %d %H:%M:%S %Y", localtime(&s_lastcheck));
    printf("\tLast Checked: %s\n", buffer);
    free(buffer);

    buffer = (char *) malloc(sizeof(char) * 80); // 80 is the max length of a string
    strftime(buffer, sizeof(char) * 80, "%a %b %d %H:%M:%S %Y", localtime(&s_mtime));
    printf("\tLast Mounted: %s\n", buffer);
    free(buffer);

    buffer = (char *) malloc(sizeof(char) * 80); // 80 is the max length of a string
    strftime(buffer, sizeof(char) * 80, "%a %b %d %H:%M:%S %Y", localtime(&s_wtime));
    printf("\tLast Written: %s\n", buffer);
    free(buffer);
}