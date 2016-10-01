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

#define QUEUE_T CAT2(NAME, _queue_t)
#define QUEUE_NODE_T CAT2(NAME, _queue_node_t)

typedef struct QUEUE_T QUEUE_T;

QUEUE_T* CAT3(new_, NAME, _queue)(void);
QUEUE_T* CAT2(NAME, _queue_enqueue)(QUEUE_T* queue, TYPE data);
TYPE CAT2(NAME, _queue_dequeue)(QUEUE_T* queue);
char CAT2(NAME, _queue_is_empty)(QUEUE_T* queue);
void CAT2(NAME, _queue_clear)(QUEUE_T* queue);
void CAT2(NAME, _queue_free)(QUEUE_T* queue);
