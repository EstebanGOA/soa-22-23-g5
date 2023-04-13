#include "ext2_metadata.h"


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

    if(lseek(fd, SUPERBLOCK_OFFSET + INODE_SIZE_OFFSET, SEEK_SET) < 0)
    {
        printf("Error: could not seek to superblock");
        exit(1);
    }

    bytes_read = read(fd, &metadata->s_inode_size, sizeof(short int));
    if (bytes_read < 0)
    {
        printf("Error: could not read file, bytes read: %d", bytes_read);
        exit(1);
    }

    if(lseek(fd, SUPERBLOCK_OFFSET, SEEK_SET) < 0)
    {
        printf("Error: could not seek to superblock");
        exit(1);
    }

    bytes_read = read(fd, &metadata->s_inodes_count, sizeof(int));
    if (bytes_read < 0)
    {
        printf("Error: could not read file, bytes read: %d", bytes_read);
        exit(1);
    }

    if(lseek(fd, SUPERBLOCK_OFFSET + INODE_FIRST_INO_OFFSET, SEEK_SET) < 0)
    {
        printf("Error: could not seek to superblock");
        exit(1);
    }

    bytes_read = read(fd, &metadata->s_first_ino, sizeof(int));
    if (bytes_read < 0)
    {
        printf("Error: could not read file, bytes read: %d", bytes_read);
        exit(1);
    }

    if(lseek(fd, SUPERBLOCK_OFFSET + FREE_INODES_COUNT_OFFSET, SEEK_SET) < 0)
    {
        printf("Error: could not seek to superblock");
        exit(1);
    }

    bytes_read = read(fd, &metadata->s_free_inodes_count, sizeof(int));
    if (bytes_read < 0)
    {
        printf("Error: could not read file, bytes read: %d", bytes_read);
        exit(1);
    }
}

void EXT2_METADATA_print(EXT2_metadata *metadata)
{
    printf("Filesystem: EXT2\n\n");
    printf("INODE INFO\n");
    printf("\tSize: %d\n", metadata->s_inode_size);
    printf("\tNum inodes: %d\n", metadata->s_inodes_count);
    printf("\tFirst inode: %d\n", metadata->s_first_ino);
    printf("\tInodes per group: %d\n", metadata->s_inodes_per_group);
    printf("\tFree inodes: %d\n", metadata->s_free_inodes_count);
}