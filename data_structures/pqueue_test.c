#include <stdio.h>

#define TYPE int
#include "priority_queue.h"

int compare(int a, int b)
{
  return a - b;
}
 
int main(int argc, char *argv[])
{
  int_pqueue_t* queue = new_int_pqueue(compare);
  int_pqueue_enqueue(queue, 1);
  int_pqueue_enqueue(queue, 2);
  int_pqueue_enqueue(queue, 3);
  int_pqueue_enqueue(queue, 4);
  int_pqueue_enqueue(queue, 3);
  int_pqueue_enqueue(queue, 2);
  int_pqueue_enqueue(queue, 1);

  while (!int_pqueue_is_empty(queue))
  {
    printf("%d\n", int_pqueue_dequeue(queue));
  }
  return 0;
}
