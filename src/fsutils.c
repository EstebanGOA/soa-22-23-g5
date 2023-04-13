#include <stdio.h>

#include "../lib/ext2_metadata.h"
#include "../lib/fat16_metadata.h"

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        printf("Usage: ./fsutils --info <file system>\n");
        exit(1);
    }
    
    // TODO: identify file format



    // TODO: apply algorithm to extract metadata informationº
    EXT2_metadata metadata = EXT2_METADATA_init(argv[2]);
    EXT2_METADATA_print(metadata);
    return 0;

        // TODO: apply algorithm to extract metadata information
    FAT16 fat16 = FAT16_METADATA_init(argv[2]);
    FAT16_METADATA_print(fat16);
    return 0;

}