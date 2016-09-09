/**** No Header guard as this file can be included multiple times with a different type ****/
/****                                                                                   ****/
/****                                   BE CAREFUL                                      ****/
/****                                                                                   ****/

#define CAT2_kernel(a, b) a ## b
#define CAT2(a, b) CAT2_kernel(a, b)
#define CAT3_kernel(a, b, c) a ## b ## c
#define CAT3(a, b, c) CAT3_kernel(a, b, c)

/* Type defined by compiler flags in the macro TYPE */
/* Optionally, the queue name new_[queue name]_queue() for eg, is defined in NAME */

#ifndef NAME
#define NAME TYPE
#endif

#define QUEUE_T CAT2(NAME, _pqueue_t)
#define QUEUE_NODE_T CAT2(NAME, _pqueue_node_t)

typedef struct QUEUE_T QUEUE_T;

QUEUE_T* CAT3(new_, NAME, _pqueue)(int (*compare)(TYPE a, TYPE b));
QUEUE_T* CAT2(NAME, _pqueue_enqueue)(QUEUE_T* queue, TYPE data);
TYPE CAT2(NAME, _pqueue_dequeue)(QUEUE_T* queue);
					    QUEUE_T* CAT2(NAME, _pqueue_update_priority)(QUEUE_T* queue, TYPE data, char (*equals)(TYPE a, TYPE b));
char CAT2(NAME, _pqueue_is_empty)(QUEUE_T* queue);
void CAT2(NAME, _pqueue_clear)(QUEUE_T* queue);
