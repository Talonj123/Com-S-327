#variables
assignment_num ?= 1.03
name = talbert_james
main_target = dungeons
folder_name = $(name).assignment-$(assignment_num)

gcc_flags = -ggdb -Wall -Werror -lm -Idata_structures -Idungeon

#Top-level targets
dungeons: dungeons.a main.c save.a pathfinding.o characters.o
	gcc $(gcc_flags) -c -o main.o main.c 
	gcc $(gcc_flags) -o $@ main.o save.a dungeons.a pathfinding.o characters.o

all: dungeons

characters.o : characters.c characters.h
	gcc $(gcc_flags) -c -o $@ characters.c

pathfinding.o: pathfinding.c
	gcc $(gcc_flags) -c -o $@ pathfinding.c

save.o: save.c
	gcc $(gcc_flags) -c -o $@ save.c

save.a: save.o room_list.o
	ld -r -o $@ save.o room_list.o
#tile_queue.o

pqueue_test: int_pqueue.o priority_queue.h pqueue_test.c
	gcc $(gcc_flags) pqueue_test.c int_pqueue.o -o $@

list_test: int_list.o list_test.c
	gcc $(gcc_flags) list_test.c int_list.o -o $@

examples: clean $(main_target)
	@echo ----------SAVED------------- >> examples
	./$(main_target) --save t1 >> examples
	sleep 1
	./$(main_target) --save t2 >> examples
	sleep 1
	./$(main_target) --save t3 >> examples
	sleep 1
	./$(main_target) --save t4 >> examples
	@echo ---------LOADED------------- >> examples
	sleep 1
	./$(main_target) --load t1 >> examples
	sleep 1
	./$(main_target) --load t2 >> examples
	sleep 1
	./$(main_target) --load t3 >> examples
	sleep 1
	./$(main_target) --load t4 >> examples
	less examples

clean: clean_general
	rm -f *.map
	rm -f *_test
	rm -f examples
	cd dungeon; make clean
	cd data_structures; make clean

#dependancies
dungeons.a: dungeon/*.c dungeon/*.c path_pqueue.o tile_list.o tile_queue.o tile_pqueue.o
	cd dungeon; make  dungeons.a;
	ld -r -o $@ dungeon/dungeons.a path_pqueue.o tile_list.o tile_queue.o tile_pqueue.o

%_list.o: data_structures/list.c
	cd data_structures; make  list TYPE=$*; cp $*_list.o ../$*_list.o
	cp data_structures/$*_list.o $*_list.o

%_queue.o: data_structures/queue.c¥
	cd data_structures; make  queue TYPE=$*; cp $*_queue.o ../$*_queue.o
	cp data_structures/$*_queue.o $*_queue.o

%_pqueue.o: data_structures/priority_queue.c
	cd data_structures; make  pqueue TYPE=$*; cp $*_pqueue.o ../$*_pqueue.o
	cp data_structures/$*_pqueue.o $*_pqueue.o

tile_queue.o:
	cd data_structures; make  queue TYPE=tile_t* NAME=tile HEADER=dungeon/dungeons.h IS_POINTER=1;
	cp data_structures/tile_queue.o tile_queue.o

tile_pqueue.o:
	cd data_structures; make  pqueue TYPE=tile_t* NAME=tile HEADER=dungeon/dungeons.h IS_POINTER=1;
	cp data_structures/tile_pqueue.o tile_pqueue.o

tile_list.o:
	cd data_structures; make  list TYPE=tile_t* NAME=tile HEADER=dungeon/dungeons.h IS_POINTER=1;
	cp data_structures/tile_list.o tile_list.o

room_list.o:
	cd data_structures; make  list TYPE=rectangle_t NAME=room HEADER=dungeon/coordinates.h;
	cp data_structures/room_list.o room_list.o

path_pqueue.o: dungeon/dungeons_private.h data_structures/priority_queue.c
	cd data_structures; make  pqueue TYPE=tile_dijkstra_t* NAME=path HEADER=dungeon/dungeons_private.h IS_POINTER=1; cp path_pqueue.o ../path_pqueue.o

monster_list.o:
	cd data_structures; make  list TYPE=monster_t* NAME=monster HEADER=characters.h;
	cp data_structures/monster_list.o monster_list.o

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
