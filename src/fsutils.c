#include <stdio.h>

#include "../lib/ext2_module.h"
#include "../lib/fat16_module.h"

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("Usage: ./fsutils --info <file system>\n");
        exit(1);
    }

    int fd = open(argv[2], O_RDONLY);
    if (fd < 0)
    {
        printf("Error: file not found");
        exit(1);
    }

    if (strcmp(argv[1], "--info") == 0)
    {
        if (EXT2_MODULE_isEXT2(fd))
        {
            EXT2_metadata metadata = EXT2_MODULE_getMetadata(fd);
            EXT2_MODULE_printMetadata(metadata);
        }
        else if (FAT16_MODULE_isFAT16(fd))
        {
            FAT16_metadata metadata = FAT16_MODULE_init(fd);
            FAT16_MODULE_print(metadata);
        }
    }
    else if (strcmp(argv[1], "--tree") == 0)
    {
        if (EXT2_MODULE_isEXT2(fd))
        {
            EXT2_MODULE_showFilesystem(fd);
        }
        else if (FAT16_MODULE_isFAT16(fd))
        {
            FAT16_metadata metadata = FAT16_MODULE_init(fd);
            FAT16_MODULE_makeTree(fd, metadata, FAT16_MODULE_getRootDirection(metadata), 0);
        }
    }
    else if (strcmp(argv[1], "--cat") == 0)
    {
        // TODO: implement cat
        if (FAT16_MODULE_isFAT16(fd))
        {
            FAT16_metadata metadata = FAT16_MODULE_init(fd);
            FAT16_MODULE_searchFile(fd, argv[3], metadata, FAT16_MODULE_getRootDirection(metadata)) == 1 ? 0 : printf("File not found");
        }
    }
    else
    {
        printf("Error: invalid option");
        exit(1);
    }

    close(fd);
    return 0;
}