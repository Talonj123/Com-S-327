#variables
assignment_num ?= 1.04
name = talbert_james
main_target = dungeons
folder_name = $(name).assignment-$(assignment_num)

gcc_flags = -ggdb -Wall -Werror -lm -Idata_structures -Idungeon -pg -lncurses	

#Top-level targets
dungeons: dungeons.a main.c save.a pathfinding.o characters.o gameflow.a
	gcc $(gcc_flags) -c -o main.o main.c
	gcc $(gcc_flags) -o $@ main.o save.a dungeons.a pathfinding.o characters.o gameflow.a

all: dungeons

examples: clean $(main_target)
	@echo ----------SAVED------------- >> examples
	./$(main_target) >> example1
	./$(main_target) >> example2
	less example1
	less example2

clean: clean_general
	rm -f *.map
	rm -f *_test
	rm -f example*
	rm -f *.out
	cd dungeon; make clean
	cd data_structures; make clean

#dependancies
dungeons.a: dungeon/*.c dungeon/*.h path_pqueue.o point_list.o point_queue.o point_pqueue.o
	cd dungeon; make  dungeons.a;
	ld -r -o $@ dungeon/dungeons.a path_pqueue.o point_list.o point_queue.o point_pqueue.o

event_pqueue.o: data_structures/priority_queue.c data_structures/priority_queue.h gameflow.h
	cd data_structures; make pqueue TYPE=event_t* NAME=event HEADER=gameflow.h
	cp data_structures/event_pqueue.o event_pqueue.o

point_queue.o: data_structures/queue.c data_structures/queue.h dungeon/coordinates.h
	cd data_structures; make queue TYPE=point_t NAME=point HEADER=dungeon/dungeons.h;
	cp data_structures/point_queue.o point_queue.o

point_pqueue.o: data_structures/priority_queue.c data_structures/priority_queue.h dungeon/coordinates.h
	cd data_structures; make pqueue TYPE=point_t NAME=point HEADER=dungeon/dungeons.h;
	cp data_structures/point_pqueue.o point_pqueue.o

point_list.o: data_structures/list.h data_structures/list.c dungeon/coordinates.h
	cd data_structures; make list TYPE=point_t NAME=point HEADER=dungeon/dungeons.h;
	cp data_structures/point_list.o point_list.o

room_list.o: data_structures/list.h data_structures/list.c dungeon/coordinates.h
	cd data_structures; make list TYPE=rectangle_t NAME=room HEADER=dungeon/coordinates.h;
	cp data_structures/room_list.o room_list.o

path_pqueue.o: data_structures/priority_queue.c data_structures/priority_queue.h dungeon/dungeons_private.h
	cd data_structures; make pqueue TYPE=tile_dijkstra_t* NAME=path HEADER=dungeon/dungeons_private.h IS_POINTER=1; cp path_pqueue.o ../path_pqueue.o

monster_list.o: data_structures/list.h data_structures/list.c characters.h
	cd data_structures; make  list TYPE=monster_t* NAME=monster HEADER=characters.h;
	cp data_structures/monster_list.o monster_list.o

characters.o : characters.c characters.h
	gcc $(gcc_flags) -c -o $@ characters.c

pathfinding.o: pathfinding.c
	gcc $(gcc_flags) -c -o $@ pathfinding.c

save.a: save.c room_list.o
	gcc $(gcc_flags) -c -o save.o save.c
	ld -r -o $@ save.o room_list.o

gameflow.a: gameflow.c gameflow.h event_pqueue.o
	gcc $(gcc_flags) -c -o gameflow.o gameflow.c
	ld -r -o $@ gameflow.o event_pqueue.o

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
