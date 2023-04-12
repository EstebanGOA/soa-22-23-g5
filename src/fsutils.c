#include <stdio.h>

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        printf("Usage: ./fsutils --info <file system>\n");
        exit(1);
    }
    
    // TODO: identify file format

    // TODO: apply algorithm to extract metadata information

    return -1;
}