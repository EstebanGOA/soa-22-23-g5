#include "fat16_metadata.h"

FAT16 FAT16_METADATA_init(char *path) {

    int fd;
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file %s", path);
        exit(0);   
    }

    FAT16 fat16;
    char *buffer = malloc(512);
    int n;
    
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