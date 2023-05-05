#ifndef _FAT16_MODULE_H
#define _FAT16_MODULE_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

#define NUMBER_ILEGAL_CHARS 16

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
} FAT16_metadata;


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


int FAT16_MODULE_isFAT16(int fd);

FAT16_metadata FAT16_MODULE_init(int fd);

void FAT16_MODULE_print(FAT16_metadata);

int FAT16_MODULE_getRootDirection(FAT16_metadata);

int FAT16_MODULE_getDirection(FAT16_metadata, int);

int FAT16_MODULE_checkStatus(char);

FAT16_entry FAT16_MODULE_readEntry(int);

void FAT16_MODULE_makeTree(int fd, FAT16_metadata, int, int);

uint8_t* FAT16_MODULE_toLower(uint8_t *);

void FAT16_MODULE_showEntry(FAT16_entry, int);

int FAT16_MODULE_searchFile(int fd, char *filename, FAT16_metadata metadata, int base_direction);

char* concat(char *filename, char *extension);

void FAT16_MODULE_readFile(int fd, int direction, FAT16_metadata metadata);

#endif