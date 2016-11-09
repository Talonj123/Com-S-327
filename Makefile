#variables
assignment_num ?= 1.07
name = talbert_james
main_target = dungeons
folder_name = $(name).assignment-$(assignment_num)

g++_flags = -ggdb -Wall -Werror -lm -Idata_structures -Idungeon -lncurses	

#Top-level targets
dungeons: dungeons.a main.cpp save.a pathfinding.o characters.o gameflow.a io.o generation.o items.o
	g++ $(g++_flags) -c -o main.o main.cpp
	g++ $(g++_flags) -o $@ main.o save.a dungeons.a pathfinding.o characters.o gameflow.a io.o generation.o items.o

all: dungeons

examples: clean $(main_target)
	@echo ----------SAVED------------- >> examples
	./$(main_target) >> example1
	./$(main_target) >> example2
	less example1
	less example2

clean: clean_general
	rm -f rlg327
	rm -f *.map
	rm -f *_test
	rm -f example*
	rm -f *.out
	cd dungeon; make clean

#dependancies
dungeons.a: dungeon/dungeons.cpp dungeon/coordinates.cpp dungeon/dungeons_private.h dungeon/dungeon_independant.h dungeon/dungeons.hpp dungeon/coordinates.h pqueue.hpp
	cd dungeon; make  dungeons.a;
	ld -r -o $@ dungeon/dungeons.a

characters.o : characters.cpp characters.h
	g++ $(g++_flags) -c -o $@ characters.cpp

pathfinding.o: pathfinding.cpp characters.h pqueue.hpp
	g++ $(g++_flags) -c -o $@ pathfinding.cpp

io.o: io.cpp io.h
	g++ $(g++_flags) -c -o $@ io.cpp

save.a: save.cpp save.h
	g++ $(g++_flags) -c -o save.o save.cpp
	ld -r -o $@ save.o

gameflow.a: gameflow.cpp gameflow.h pqueue.hpp
	g++ $(g++_flags) -c -o gameflow.o gameflow.cpp
	ld -r -o $@ gameflow.o

generation.o: generation.cpp items.hpp generation.hpp characters.h
	g++ $(g++_flags) -c -o $@ generation.cpp

items.o: items.cpp items.hpp generation.hpp
	g++ $(g++_flags) -c -o $@ items.cpp

#Common targets
clean_general:
        # object files/binaries
	rm -f *.o $(main_target) *.o.i *.a
        # emacs buffers
	rm -f *~ \#*\#
        # old submission data 
	rm -f $(name).*.tar.gz
	rm -r -f ../$(folder_name)
	rm -r -f $(folder_name)
	rm -r -f test_folder
        # .directory file that keeps popping up
	rm -f .directory
        # example folder
	rm -r -f examples

submission: $(main_target) clean Makefile CHANGELOG README
	cp -R . ../$(folder_name)
	tar cvfz $(folder_name).tar.gz ../$(folder_name) --exclude='.git' --exclude='.gitignore' --exclude='.directory'
	rm -r -f ../$(folder_name)

test: submission
	gzip -dc $(folder_name).tar.gz | tar xvf -
	cp -f -r $(folder_name) test_folder
	rm -r -f $(folder_name)
	cd test_folder; make examples
	rm -fr test_folder

##shortcut
sub: submission
