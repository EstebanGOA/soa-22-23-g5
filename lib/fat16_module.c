#include "fat16_module.h"

int FAT16_MODULE_isFAT16(int fd)
{

    unsigned short int bpb_bps;      // bytes per sector
    unsigned short int bpb_rec;      // root entries count
    short int bpb_fatsz16;           // FAT size in sectors
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

    if (bpb_fatsz16 != 0)
    {
        data_sectors = bpb_totsec16 - (bpb_rsc + (bpb_numfats * bpb_fatsz16) + root_dir_sectors);
        count_clusters = data_sectors / bpb_spc;
        return (count_clusters > 4085 && count_clusters < 65525) ? 1 : -1;
    }
    return -1;
}

FAT16_metadata FAT16_MODULE_init(int fd)
{
    FAT16_metadata fat16;
    char *buffer = malloc(512);
    int n;

    lseek(fd, 0, SEEK_SET);
    n = read(fd, buffer, 512);
    if (n < 0)
    {
        printf("Error reading file");
        exit(0);
    }

    memcpy(fat16.system_name, buffer + 3, 8 + 1);
    fat16.system_name[8] = '\0';
    memcpy(&fat16.sector_size, buffer + 11, 2);
    memcpy(&fat16.sectors_per_cluster, buffer + 13, 1);
    memcpy(&fat16.reserved_sectors, buffer + 14, 2);
    memcpy(&fat16.number_of_fats, buffer + 16, 1);
    memcpy(&fat16.root_dir_entries, buffer + 17, 2);
    memcpy(&fat16.sectors_per_fat, buffer + 22, 2);
    memcpy(fat16.label, buffer + 43, 11 + 1);
    fat16.label[11] = '\0';
    free(buffer);

    return fat16;
}

void FAT16_MODULE_print(FAT16_metadata fat16)
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

int FAT16_MODULE_getRootDirection(FAT16_metadata fat16)
{
    return (fat16.reserved_sectors + (fat16.sectors_per_fat * fat16.number_of_fats)) * fat16.sector_size + 32;
}

int FAT16_MODULE_getDirection(FAT16_metadata fat16, int cluster)
{
    int RootDirSectors = ((fat16.root_dir_entries * 32) + (fat16.sector_size - 1)) / fat16.sector_size;
    int FirstDataSector = fat16.reserved_sectors + (fat16.sectors_per_fat * fat16.number_of_fats) + RootDirSectors;
    int FirstSectorOfCluster = (((cluster - 2) * fat16.sectors_per_cluster) + FirstDataSector) * fat16.sector_size;
    return FirstSectorOfCluster;
}

int FAT16_MODULE_checkStatus(char c)
{
    char ilegal[NUMBER_ILEGAL_CHARS] = {0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, 0x7C};

    for (int i = 0; i < NUMBER_ILEGAL_CHARS; i++)
    {
        if (c == ilegal[i])
            return 1;
    }
    return 0;
}

FAT16_entry FAT16_MODULE_readEntry(int fd)
{
    FAT16_entry entry;

    read(fd, entry.name, 8);
    entry.name[8] = '\0';
    for (int i = 0; i < 8; i++)
    {
        if (entry.name[i] == 0x20)
        {
            entry.name[i] = '\0';
        }
    }

    read(fd, entry.extension, 3);
    entry.extension[3] = '\0';

    read(fd, &entry.attributes, 1);

    lseek(fd, 14, SEEK_CUR);
    read(fd, &entry.first_cluster_low, 2);

    lseek(fd, 4, SEEK_CUR);
    return entry;
}

void FAT16_MODULE_makeTree(int fd, FAT16_metadata fat16, int base_direction, int depth)
{
    FAT16_entry entry;
    int direction = base_direction;
    lseek(fd, direction, SEEK_SET);
    while (1)
    {
        // Read the entry
        entry = FAT16_MODULE_readEntry(fd);
        // Size of an entry
        direction += 32;
        if (entry.name[0] == 0x00)
            break;
        if (entry.name[0] == 0xE5)
            continue;
        if (entry.attributes == 0x0F)
            continue;
        if (FAT16_MODULE_checkStatus(entry.name[0]))
            continue;

        FAT16_MODULE_showEntry(entry, depth);

        // Check if it's a directory
        if (entry.attributes == 0x10)
        {
            int dir = FAT16_MODULE_getDirection(fat16, entry.first_cluster_low);
            FAT16_MODULE_makeTree(fd, fat16, dir, depth + 1);
            lseek(fd, direction, SEEK_SET);
        }
    }
}

uint8_t *FAT16_MODULE_toLower(uint8_t *s)
{
    int length = strlen((char *)s);
    for (int i = 0; i < length; i++)
    {
        if (s[i] == '\0')
            break;

        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] += 32;
    }
    return s;
}

void FAT16_MODULE_showEntry(FAT16_entry entry, int depth)
{
    if (depth > 0)
        printf("|");
    for (int i = 0; i < depth; i++)
        printf("    ");

    printf("|-- %s", FAT16_MODULE_toLower(entry.name));
    if (entry.extension[0] != 0x20)
    {
        printf(".%s", FAT16_MODULE_toLower(entry.extension));
    }
    printf("\n");
}

char *concat(char *filename, char *extension)
{
    uint8_t i = 0;
    uint8_t buffer_size = 1;
    char *buffer = malloc(sizeof(char) * buffer_size);
    buffer[0] = '\0';

    for (i = 0; i < strlen(filename); i++)
    {
        if (filename[i] != '\0' && filename[i] != 0x20)
        {
            buffer_size++;
            buffer = realloc(buffer, sizeof(char) * buffer_size);
            buffer[buffer_size - 2] = filename[i];
            buffer[buffer_size - 1] = '\0';
        }
    }

    if (extension[0] != 0x20)
    {
        buffer_size++;
        buffer = realloc(buffer, sizeof(char) * buffer_size);
        buffer[buffer_size - 2] = '.';
        buffer[buffer_size - 1] = '\0';

        for (i = 0; i < strlen(extension); i++)
        {
            if (extension[i] != '\0' && extension[i] != 0x20)
            {
                buffer_size++;
                buffer = realloc(buffer, sizeof(char) * buffer_size);
                buffer[buffer_size - 2] = extension[i];
                buffer[buffer_size - 1] = '\0';
            }
        }
    }

    return buffer;
}

int FAT16_MODULE_searchFile(int fd, char *filename, FAT16_metadata metadata, int direction)
{
    FAT16_entry entry;
    int first_cluster_low;
    char *buffer;
    lseek(fd, direction, SEEK_SET);
    while (1)
    {
        // Read the entry
        entry = FAT16_MODULE_readEntry(fd);
        // Size of an entry
        direction += 32;
        if (entry.name[0] == 0x00)
            break;
        if (entry.name[0] == 0xE5)
            continue;
        if (entry.attributes == 0x0F)
            continue;
        if (FAT16_MODULE_checkStatus(entry.name[0]))
            continue;

        buffer = concat((char *) FAT16_MODULE_toLower(entry.name), (char *) FAT16_MODULE_toLower(entry.extension));
        if (strcmp(buffer, filename) == 0)
        {
            first_cluster_low = FAT16_MODULE_getDirection(metadata, entry.first_cluster_low);
            FAT16_MODULE_readFile(fd, first_cluster_low, metadata);
            free(buffer);
            return 1;
        }
        free(buffer);

        // Check if it's a directory
        if (entry.attributes == 0x10)
        {
            first_cluster_low = FAT16_MODULE_getDirection(metadata, entry.first_cluster_low);
            return FAT16_MODULE_searchFile(fd, filename, metadata, first_cluster_low);
            lseek(fd, direction, SEEK_SET);
        }
    }
    return -1;
}


void FAT16_MODULE_readFile(int fd, int direction, FAT16_metadata metadata) {
    int bytes_read;
    int bytes_per_sector = metadata.sector_size * metadata.sectors_per_cluster;
    int i = 1;
    char *buffer;
    
    
    lseek(fd, direction, SEEK_SET);
    buffer = malloc(sizeof(char *) * (bytes_per_sector + i));
    bytes_read = read(fd, buffer, sizeof(char *) * (bytes_per_sector + i));

    while (bytes_read == bytes_per_sector) {
        buffer = realloc(buffer, sizeof(char *) * (bytes_per_sector * i + 1));
        bytes_read = read(fd, buffer + (bytes_per_sector * i), sizeof(char *) * (bytes_per_sector * i + 1));
        i++;
    }

    buffer = realloc(buffer, sizeof(char *) * (bytes_per_sector * i - 1) + bytes_read);
    
    printf("%s\n", buffer);
    free(buffer);
    
    // TODO: If the file is bigger than one cluster, read the next cluster
    
}