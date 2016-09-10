gcc_flags = -ggdb -Wall -Werror -I..
#TYPE has the type (from command_line)
NAME=$(TYPE)
ifdef HEADER
  INCLUDE=-include $(HEADER)
else
  INCLUDE=
endif

ifdef IS_POINTER
  POINTER=-DIS_POINTER=1
else
  POINTER=
endif

all: ptr_stack.o ptr_queue.o ptr_pqueue.o ptr_list.o

ptr_%.o:
	make $* TYPE=void* NAME=ptr IS_POINTER=1

clean:
	rm -f *.o *~ *.a

$(NAME)_list.o: list.c list_internal.h
	gcc $(gcc_flags) -c -o $@ list.c -DNAME=$(NAME) -DTYPE=$(TYPE) $(POINTER) $(INCLUDE)

$(NAME)_queue.o: queue.c list_internal.h
	gcc $(gcc_flags) -c -o $@ queue.c -DNAME=$(NAME) -DTYPE=$(TYPE) $(POINTER) $(INCLUDE)

$(NAME)_pqueue.o: priority_queue.c priority_queue_internal.h
	cpp -I.. priority_queue.c -DNAME=$(NAME) -DTYPE=$(TYPE) $(POINTER) $(INCLUDE) >> $(NAME)_pqueue.o.i
	gcc $(gcc_flags) -c -o $@ priority_queue.c -DNAME=$(NAME) -DTYPE=$(TYPE) $(POINTER) $(INCLUDE)

$(NAME)_stack.o: stack.c stack_internal.h
	gcc $(gcc_flags) -c -o $@ stack.c -DNAME=$(NAME) -DTYPE=$(TYPE) $(POINTER) $(INCLUDE)

list: $(NAME)_list.o
	@echo $(TYPE) list built as $(NAME)_list.o
queue: $(NAME)_queue.o
	@echo $(TYPE) queue built as $(NAME)_queue.o
pqueue: $(NAME)_pqueue.o
	@echo $(TYPE) pqueue built as $(NAME)_pqueue.o
stack: $(NAME)_stack.o
	@echo $(TYPE) stack built as $(NAME)_stack.o
