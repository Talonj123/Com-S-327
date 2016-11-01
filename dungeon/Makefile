g++_flags = -ggdb -Wall -Werror -I..

dungeons.a: dungeons.o coordinates.o
	ld -r -o $@ dungeons.o coordinates.o

dungeons.o: dungeons.cpp coordinates.cpp coordinates.cpp dungeons.h dungeons_private.h
	g++ $(g++_flags) -c -o $@ dungeons.cpp

coordinates.o: coordinates.cpp
	g++ $(g++_flags) -c -o $@ coordinates.cpp

clean:
	rm -f *.o *~ *.a
