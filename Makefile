make: main.o clean

clean:
	rm -f *.o

main.o: fsutils.o ext2_metadata.o
	gcc ext2_metadata.o fsutils.o -o fsutils -Wall -Wextra

ext2_metadata.o: lib/ext2_metadata.c lib/ext2_metadata.h
	gcc -c lib/ext2_metadata.c -Wall -Wextra

fsutils.o: src/fsutils.c 
	gcc -c src/fsutils.c -Wall -Wextra