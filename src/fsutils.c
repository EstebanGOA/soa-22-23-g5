#include <stdio.h>
#include "../lib/ext2_metadata.h"

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        printf("Usage: ./fsutils --info <file system>\n");
        exit(1);
    }
    
    // TODO: identify file format

    // TODO: apply algorithm to extract metadata informationº
    EXT2_metadata superblock;
    EXT2_METADATA_init(&superblock, argv[2]);
    EXT2_METADATA_print(&superblock);
    return -1;
}