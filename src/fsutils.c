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

    if (EXT2_METADATA_isEXT2(fd))
    {
        EXT2_metadata metadata = EXT2_METADATA_init(fd);
        EXT2_METADATA_print(metadata);
    }
    else if (FAT16_METADATA_isFAT16(fd))
    {
        FAT16 metadata = FAT16_METADATA_init(fd);
        FAT16_METADATA_print(metadata);
    }

    close(fd);
    return 0;
}