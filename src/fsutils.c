#include <stdio.h>
#include "../lib/fat16_metadata.h"

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        printf("Usage: ./fsutils --info <file system>\n");
        exit(1);
    }
    
    // TODO: identify file format

    // TODO: apply algorithm to extract metadata information
    int fd = FAT16_METADATA_init(argv[2]);
    FAT16 fat16 = FAT16_METADATA_info(fd);
    FAT16_METADATA_print(fat16);
    return 0;

}