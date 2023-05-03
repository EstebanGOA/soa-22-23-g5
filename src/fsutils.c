#include <stdio.h>

#include "../lib/ext2_metadata.h"
#include "../lib/fat16_metadata.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./fsutils --info <file system>\n");
        exit(1);
    }

    // TODO: identify file format
    int fd = open(argv[2], O_RDONLY);
    if (fd < 0)
    {
        printf("Error: file not found");
        exit(1);
    }

    if (strcmp(argv[1], "--info") == 0)
    {
        if (EXT2_MODULE_is_EXT2(fd))
        {
            EXT2_metadata metadata = EXT2_MODULE_get_metadata(fd);
            EXT2_MODULE_print_metadata(metadata);
        }
        else if (FAT16_METADATA_isFAT16(fd))
        {
            FAT16 metadata = FAT16_METADATA_init(fd);
            FAT16_METADATA_print(metadata);
        }
    }
    else if (strcmp(argv[1], "--tree") == 0)
    {
        if (EXT2_MODULE_is_EXT2(fd))
        {
            EXT2_MODULE_show_filesystem(fd);
        }
        else if (FAT16_METADATA_isFAT16(fd))
        {
            // TODO: 
        }
    }   
    else if (strcmp(argv[1], "--cat") == 0)
    {
    }
    else
    {
        printf("Error: invalid option");
        exit(1);
    }


    close(fd);
    return 0;
}