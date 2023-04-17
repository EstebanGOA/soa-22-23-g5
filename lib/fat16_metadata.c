#include "fat16_metadata.h"

int FAT16_METADATA_isFAT16(int fd) {

    unsigned short int bpb_bps; // bytes per sector
    unsigned short int bpb_rec; // root entries count
    short int bpb_fatsz16; // FAT size in sectors
    unsigned short int bpb_totsec16; // total sectors count
    unsigned int root_dir_sectors;
    unsigned short int data_sectors;
    unsigned char bpb_spc; // sectors per cluster
    unsigned char bpb_numfats; 
    unsigned short int bpb_rsc; // reserved sectors count
    unsigned int count_clusters;

    lseek(fd, 11, SEEK_SET);
    read(fd, &bpb_bps, sizeof(unsigned short int));

    lseek(fd, 17, SEEK_SET);
    read(fd, &bpb_rec, sizeof(unsigned short int));

    lseek(fd, 19, SEEK_SET);
    read(fd, &bpb_totsec16, sizeof(unsigned short int));

    lseek(fd, 13, SEEK_SET);
    read(fd, &bpb_spc, sizeof(unsigned char));

    lseek(fd, 16, SEEK_SET);
    read(fd, &bpb_numfats, sizeof(unsigned char));

    lseek(fd, 14, SEEK_SET);
    read(fd, &bpb_rsc, sizeof(unsigned short int));

    lseek(fd, 22, SEEK_SET);
    read(fd, &bpb_fatsz16, sizeof(short int));

    root_dir_sectors = ((bpb_rec * 32) + (bpb_bps - 1)) / bpb_bps;
    
    if (bpb_fatsz16 != 0) {
        data_sectors = bpb_totsec16 - (bpb_rsc + (bpb_numfats * bpb_fatsz16) + root_dir_sectors);
        count_clusters = data_sectors / bpb_spc;
        return count_clusters < 4085 ? 1 : -1;
    }
    return -1;
}

FAT16 FAT16_METADATA_init(int fd) {
    FAT16 fat16;
    char *buffer = malloc(512);
    int n;

    lseek(fd, 0, SEEK_SET);
    n = read(fd, buffer, 512);
    if (n < 0) {
        printf("Error reading file");
        exit(0);
    }
    
    memcpy(fat16.system_name, buffer + 3, 8);
    memcpy(&fat16.sector_size, buffer + 11, 2);
    memcpy(&fat16.sectors_per_cluster, buffer + 13, 1);
    memcpy(&fat16.reserved_sectors, buffer + 14, 2);
    memcpy(&fat16.number_of_fats, buffer + 16, 1);
    memcpy(&fat16.root_dir_entries, buffer + 17, 2);
    memcpy(&fat16.sectors_per_fat, buffer + 22, 2);
    memcpy(fat16.label, buffer + 43, 11);
    free(buffer);
    close(fd);

    return fat16;
    
}

void FAT16_METADATA_print(FAT16 fat16)
{
    printf("\n------ Filesystem Information ------\n\n");
    
    printf("Filesystem: FAT16\n\n");
    
    printf("System name: %s\n", fat16.system_name);
    printf("Sector size: %d\n", fat16.sector_size);
    printf("Sectors per cluster: %d\n", fat16.sectors_per_cluster);
    printf("Reserved sectors: %d\n", fat16.reserved_sectors);
    printf("# of FATs: %d\n", fat16.number_of_fats);
    printf("Max root entries: %d\n", fat16.root_dir_entries);
    printf("Sector per FAT: %d\n", fat16.sectors_per_fat);
    printf("Label: %s\n", fat16.label);
}