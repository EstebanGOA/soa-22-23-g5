#include "ext2_metadata.h"

void lseek_or_die(int fd, int offset, int action)
{
    if(lseek(fd, SUPERBLOCK_OFFSET + offset, action) < 0)
    {
        printf("Error: could not seek to superblock");
        exit(1);
    }
}

void EXT2_METADATA_read_or_die(int fd, void *buffer, int size, char *error_msg) {
    int bytes_read = read(fd, buffer, size);
    if (bytes_read < 0)
    {
        printf("%s\n", error_msg);
        exit(1);
    }
}

void EXT2_METADATA_init(EXT2_metadata *metadata, char *path)
{
    int fd;
    int bytes_read;

    fd = open(path, O_RDONLY);
    if (fd < 0) 
    {
        printf("Error: file not found");
        exit(1);
    }
    
    // TODO: Order by constants in header file, so it's not necessary to seek multiple times

    lseek_or_die(fd, INODE_SIZE_OFFSET, SEEK_SET);
    EXT2_METADATA_read_or_die(fd, &metadata->s_inode_size, sizeof(short int), "Error: could not read inode size");

    lseek_or_die(fd, SUPERBLOCK_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_inodes_count, sizeof(int));
    if (bytes_read < 0)
    {
        printf("Error: could not read file, bytes read: %d", bytes_read);
        exit(1);
    }

    lseek_or_die(fd, FIRST_INO_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_first_ino, sizeof(int));
    if (bytes_read < 0)
    {
        printf("Error: could not read file, bytes read: %d", bytes_read);
        exit(1);
    }

    lseek_or_die(fd, FREE_INODES_COUNT_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_free_inodes_count, sizeof(int));
    if (bytes_read < 0)
    {
        printf("Error: could not read file, bytes read: %d", bytes_read);
        exit(1);
    }

    lseek_or_die(fd, LOG_BLOCK_SIZE_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_log_block_size, sizeof(int));

    lseek_or_die(fd, R_BLOCKS_COUNT_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_r_blocks_count, sizeof(int));

    lseek_or_die(fd, FREE_BLOCKS_COUNT_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_free_blocks_count, sizeof(int));

    lseek_or_die(fd, BLOCKS_COUNT_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_blocks_count, sizeof(int));

    lseek_or_die(fd, FIRST_DATA_BLOCK_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_first_data_block, sizeof(int));

    lseek_or_die(fd, BLOCKS_PER_GROUP_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_blocks_per_group, sizeof(int));

    lseek_or_die(fd, FRAGS_PER_GROUP_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_frags_per_group, sizeof(int));

    lseek_or_die(fd, VOLUME_NAME_OFFSET, SEEK_SET);
    metadata->s_volume_name = malloc(sizeof(char) * 17);
    bytes_read = read(fd, metadata->s_volume_name, sizeof(char) * 16);
    metadata->s_volume_name[16] = '\0';

    lseek_or_die(fd, LASTCHECK_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_lastcheck, sizeof(int));

    lseek_or_die(fd, MTIME_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_mtime, sizeof(int));

    lseek_or_die(fd, WTIME_OFFSET, SEEK_SET);
    bytes_read = read(fd, &metadata->s_wtime, sizeof(int));
}

void EXT2_METADATA_print(EXT2_metadata *metadata)
{  
    char *buffer = NULL;
    time_t s_lastcheck = (time_t) metadata->s_lastcheck;
    time_t s_mtime = (time_t) metadata->s_mtime;
    time_t s_wtime = (time_t) metadata->s_wtime;
    
    printf("Filesystem: EXT2\n");
    printf("\nINODE INFO\n");
    printf("\tSize: %d\n", metadata->s_inode_size);
    printf("\tNum inodes: %d\n", metadata->s_inodes_count);
    printf("\tFirst inode: %d\n", metadata->s_first_ino);
    printf("\tInodes per group: %d\n", metadata->s_inodes_per_group);
    printf("\tFree inodes: %d\n", metadata->s_free_inodes_count);

    printf("\nINFO BLOCK\n");
    printf("\tBlock size: %d\n", (1024 << metadata->s_blocks_count));
    printf("\tReserved blocks: %d\n", metadata->s_r_blocks_count);
    printf("\tFree blocks: %d\n", metadata->s_free_blocks_count);
    printf("\tTotal blocks: %d\n", metadata->s_blocks_count);
    printf("\tFirst block: %d\n", metadata->s_first_data_block);
    printf("\tGroup blocks: %d\n", metadata->s_blocks_per_group);
    printf("\tGroup flags: %d\n", metadata->s_frags_per_group);

    
    printf("\nINFO VOLUME\n");
    printf("\tVolume name: %s\n", metadata->s_volume_name);

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