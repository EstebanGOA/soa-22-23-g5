#ifndef _FAT16_METADATA_H
#define _FAT16_METADATA_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>


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


typedef struct {
    uint8_t name[9];
    uint8_t extension[4];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_tenth;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t last_modification_time;
    uint16_t last_modification_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} FAT16_entry;

#define NUMBER_ILEGAL_CHARS 16

int FAT16_METADATA_isFAT16(int fd);

FAT16 FAT16_METADATA_init(int fd);

void FAT16_METADATA_print(FAT16);

int FAT16_getRootDirection(FAT16);

int FAT16_getDirection(FAT16, int);

int FAT16_checkStatus(char);

FAT16_entry FAT16_readEntry(int);

void FAT16_makeTree(int fd, FAT16, int, int);

uint8_t* FAT16_toLower(uint8_t *);

void FAT16_SHOW_ENTRY(FAT16_entry, int);

#endif