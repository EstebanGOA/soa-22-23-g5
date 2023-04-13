make: main.o clean

main.o: fsutils.o fat16_metadata.o ext2_metadata.o
	gcc fsutils.o fat16_metadata.o ext2_metadata.o -o ./fsutils

fsutils.o: fsutils.c
	gcc -c fsutils.c

fat16_metadata.o: ../lib/fat16_metadata.c ../lib/fat16_metadata.h
	gcc -c ../lib/fat16_metadata.c 

ext2_metadata.o: ../lib/ext2_metadata.c ../lib/ext2_metadata.h
	gcc -c ../lib/ext2_metadata.c 
	
clean: 
	rm *.o
