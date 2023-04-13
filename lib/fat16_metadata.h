#ifndef _FAT16_METADATA_H
#define _FAT16_METADATA_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


typedef struct {
    char system_name[8];
    unsigned short sector_size;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char number_of_fats;
    unsigned short root_dir_entries;
    unsigned short small_sectors;
    unsigned char media_descriptor;
    unsigned short sectors_per_fat;
    char label[11];
} FAT16;



int FAT16_METADATA_init( char *);

FAT16 FAT16_METADATA_info(int);

void FAT16_METADATA_print(FAT16);

#endif

