#include "ext2_metadata.h"

EXT2_group EXT2_MODULE_get_group_descriptors(int fd, int block_size, int offset)
{
    EXT2_group group;
    int bytes_read;
    int block_offset = 0;
    block_offset = lseek(fd, SUPERBLOCK_OFFSET + block_size + offset * sizeof(EXT2_group), SEEK_SET);
    if (block_offset == -1)
    {
        printf("Error: could not seek to group descriptor table\n");
        exit(1);
    }
    bytes_read = read(fd, &group, sizeof(group));
    if (bytes_read != sizeof(EXT2_group))
    {
        printf("Error: could not read group descriptors\n");
        exit(1);
    }
    return group;
}

EXT2_inode EXT2_MODULE_get_inode(int fd, int position, EXT2_group group, EXT2_metadata metadata)
{
    EXT2_inode inode;
    int bytes_read = 0;
    int block_size = 0, block_offset = 0;
    block_size = (1024 << metadata.s_log_block_size);
    block_offset = lseek(fd, (SUPERBLOCK_OFFSET + (group.bg_inode_table - 1) * block_size) + position, SEEK_SET);
    if (block_offset == -1)
    {
        printf("Error: could not seek to inode\n");
        exit(1);
    }
    bytes_read = read(fd, &inode, sizeof(EXT2_inode));
    if (bytes_read != sizeof(EXT2_inode))
    {
        printf("Error: could not read inode\n");
        exit(1);
    }
    return inode;
}

void EXT2_MODULE_read_inode(int fd, EXT2_inode inode, EXT2_metadata metadata, int level)
{
    int offset = 0, acum = 0;
    int block_size = (1024 << metadata.s_log_block_size);
    EXT2_entry entry;
    EXT2_inode sub_inode;
    EXT2_group sub_group;
    int block_group = 0;
    int local_inode_index = 0;

    offset = SUPERBLOCK_OFFSET + (inode.i_block[0] - 1) * block_size;
    do
    {
        lseek(fd, offset, SEEK_SET);
        read(fd, &entry.inode, sizeof(entry.inode));
        read(fd, &entry.rec_len, sizeof(entry.rec_len));
        read(fd, &entry.name_len, sizeof(entry.name_len));
        read(fd, &entry.file_type, sizeof(entry.file_type));
        entry.name = malloc(entry.name_len + 1);
        read(fd, entry.name, entry.name_len);
        entry.name[(int)entry.name_len] = '\0';

        offset += entry.rec_len;
        acum += entry.rec_len;

        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0)
            continue;

        if (entry.file_type == EXT2_FT_DIR || entry.file_type == EXT2_FT_REG_FILE)
        {
            if (level > 0)
            {
                printf("|");
            }
            for (int i = 0; i < level; i++)
                printf("\t");
            printf("|-- %s\n", entry.name);
        }

        if (entry.file_type == EXT2_FT_DIR)
        {
            block_group = (entry.inode - 1) / metadata.s_inodes_per_group;
            local_inode_index = (entry.inode - 1) % metadata.s_inodes_per_group;
            sub_group = EXT2_MODULE_get_group_descriptors(fd, block_size, block_group);
            sub_inode = EXT2_MODULE_get_inode(fd, local_inode_index * metadata.s_inode_size, sub_group, metadata);
            EXT2_MODULE_read_inode(fd, sub_inode, metadata, level + 1);
        }
        free(entry.name);
    } while (acum < inode.i_size);
}

void EXT2_MODULE_show_filesystem(int fd)
{
    EXT2_group group;
    EXT2_metadata metadata;
    EXT2_inode inode;
    uint32_t block_size;

    metadata = EXT2_MODULE_get_metadata(fd);
    block_size = (1024 << metadata.s_log_block_size);

    group = EXT2_MODULE_get_group_descriptors(fd, block_size, 0);
    inode = EXT2_MODULE_get_inode(fd, 2 * sizeof(EXT2_inode), group, metadata);
    EXT2_MODULE_read_inode(fd, inode, metadata, 0);
}

int EXT2_MODULE_is_EXT2(int fd)
{
    uint16_t magic_number;
    lseek(fd, SUPERBLOCK_OFFSET + S_MAGIC_OFFSET, SEEK_SET);
    if (read(fd, &magic_number, sizeof(uint16_t)) < 0)
    {
        printf("Error reading file");
        exit(1);
    }
    return magic_number == EXT2_MAGIC_NUMBER;
}

void EXT2_MODULE_lseek_or_die(int fd, int offset, int action)
{
    if (lseek(fd, SUPERBLOCK_OFFSET + offset, action) < 0)
    {
        printf("Error: could not seek to superblock");
        exit(1);
    }
}

void EXT2_MODULE_read_or_die(int fd, void *buffer, int size, char *error_msg)
{
    int bytes_read = read(fd, buffer, size);
    if (bytes_read < 0)
    {
        printf("%s\n", error_msg);
        exit(1);
    }
}

EXT2_metadata EXT2_MODULE_get_metadata(int fd)
{
    EXT2_metadata metadata;

    EXT2_MODULE_lseek_or_die(fd, 0, SEEK_SET);
    EXT2_MODULE_read_or_die(fd, &metadata.s_inodes_count, sizeof(uint32_t), "Error: could not read inodes count");
    EXT2_MODULE_read_or_die(fd, &metadata.s_blocks_count, sizeof(uint32_t), "Error: could not read blocks count");
    EXT2_MODULE_read_or_die(fd, &metadata.s_r_blocks_count, sizeof(uint32_t), "Error: could not read reserved blocks count");
    EXT2_MODULE_read_or_die(fd, &metadata.s_free_blocks_count, sizeof(uint32_t), "Error: could not read free blocks count");
    EXT2_MODULE_read_or_die(fd, &metadata.s_free_inodes_count, sizeof(uint32_t), "Error: could not read free inodes count");
    EXT2_MODULE_read_or_die(fd, &metadata.s_first_data_block, sizeof(uint32_t), "Error: could not read first data block");
    EXT2_MODULE_read_or_die(fd, &metadata.s_log_block_size, sizeof(uint32_t), "Error: could not read log block size");

    EXT2_MODULE_lseek_or_die(fd, BLOCKS_PER_GROUP_OFFSET, SEEK_SET);
    EXT2_MODULE_read_or_die(fd, &metadata.s_blocks_per_group, sizeof(uint32_t), "Error: could not read blocks per group");
    EXT2_MODULE_read_or_die(fd, &metadata.s_frags_per_group, sizeof(uint32_t), "Error: could not read frags per group");
    EXT2_MODULE_read_or_die(fd, &metadata.s_inodes_per_group, sizeof(uint32_t), "Error: could not read inodes per group");
    EXT2_MODULE_read_or_die(fd, &metadata.s_mtime, sizeof(uint32_t), "Error: could not read mount time");
    EXT2_MODULE_read_or_die(fd, &metadata.s_wtime, sizeof(uint32_t), "Error: could not read write time");

    EXT2_MODULE_lseek_or_die(fd, LASTCHECK_OFFSET, SEEK_SET);
    EXT2_MODULE_read_or_die(fd, &metadata.s_lastcheck, sizeof(uint32_t), "Error: could not read last check");

    EXT2_MODULE_lseek_or_die(fd, FIRST_INO_OFFSET, SEEK_SET);
    EXT2_MODULE_read_or_die(fd, &metadata.s_first_ino, sizeof(uint32_t), "Error: could not read first inode");
    EXT2_MODULE_read_or_die(fd, &metadata.s_inode_size, sizeof(uint16_t), "Error: could not read inode size");

    EXT2_MODULE_lseek_or_die(fd, VOLUME_NAME_OFFSET, SEEK_SET);
    EXT2_MODULE_read_or_die(fd, &metadata.s_volume_name, sizeof(char) * 16, "Error: could not read volume name");

    return metadata;
}

void EXT2_MODULE_print_metadata(EXT2_metadata metadata)
{
    char *buffer = NULL;
    time_t s_lastcheck = (time_t)metadata.s_lastcheck;
    time_t s_mtime = (time_t)metadata.s_mtime;
    time_t s_wtime = (time_t)metadata.s_wtime;

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

    buffer = (char *)malloc(sizeof(char) * 80); // 80 is the max length of a string
    strftime(buffer, sizeof(char) * 80, "%a %b %d %H:%M:%S %Y", localtime(&s_lastcheck));
    printf("\tLast Checked: %s\n", buffer);
    free(buffer);

    buffer = (char *)malloc(sizeof(char) * 80); // 80 is the max length of a string
    strftime(buffer, sizeof(char) * 80, "%a %b %d %H:%M:%S %Y", localtime(&s_mtime));
    printf("\tLast Mounted: %s\n", buffer);
    free(buffer);

    buffer = (char *)malloc(sizeof(char) * 80); // 80 is the max length of a string
    strftime(buffer, sizeof(char) * 80, "%a %b %d %H:%M:%S %Y", localtime(&s_wtime));
    printf("\tLast Written: %s\n", buffer);
    free(buffer);
}