make: main.o clean

clean:
	rm -f *.o

main.o: fsutils.o ext2_module.o fat16_module.o
	gcc ext2_module.o fat16_module.o fsutils.o -o fsutils -Wall -Wextra

ext2_module.o: lib/ext2_module.c lib/ext2_module.h
	gcc -c lib/ext2_module.c -Wall -Wextra

fat16_module.o: lib/fat16_module.c lib/fat16_module.h
	gcc -c lib/fat16_module.c -Wall -Wextra

fsutils.o: src/fsutils.c 
	gcc -c src/fsutils.c -Wall -Wextra