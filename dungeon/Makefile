gcc_flags = -ggdb -Wall -Werror -I..

dungeons.a: dungeons.o coordinates.o
	ld -r -o $@ dungeons.o coordinates.o

dungeons.o: dungeons.c coordinates.c coordinates.c dungeons.h dungeons_private.h
	gcc $(gcc_flags) -c -o $@ dungeons.c

coordinates.o: coordinates.c
	gcc $(gcc_flags) -c -o $@ coordinates.c

clean:
	rm -f *.o *~ *.a
