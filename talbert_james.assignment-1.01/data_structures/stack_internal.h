/**** No Header guard as this file can be included multiple times with a different type ****/
/****                                                                                   ****/
/****                                   BE CAREFUL                                      ****/
/****                                                                                   ****/

#define CAT2_kernel(a, b) a ## b
#define CAT2(a, b) CAT2_kernel(a, b)
#define CAT3_kernel(a, b, c) a ## b ## c
#define CAT3(a, b, c) CAT3_kernel(a, b, c)

/* Type defined by compiler flags in the macro TYPE */
/* Optionally, the stack name new_[stack name]_stack() for eg, is defined in NAME */

#ifndef NAME
#define NAME TYPE
#endif

#define STACK_T CAT2(NAME, _stack_t)
#define STACK_NODE_T CAT2(NAME, _stack_node_t)

typedef struct STACK_NODE_T STACK_NODE_T;
typedef struct STACK_T STACK_T;

STACK_T* CAT3(new_, NAME, _stack)(void);
STACK_T* CAT2(NAME, _stack_push)(STACK_T* stack, TYPE data);
TYPE CAT2(NAME, _stack_pop)(STACK_T* stack);
char CAT2(NAME, _stack_is_empty)(STACK_T* stack);
void CAT2(NAME, _stack_clear)(STACK_T* stack);
