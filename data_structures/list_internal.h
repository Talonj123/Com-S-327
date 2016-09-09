/**** No Header guard as this file can be included multiple times with a different type ****/
/****                                                                                   ****/
/****                                   BE CAREFUL                                      ****/
/****                                                                                   ****/

#define CAT2_kernel(a, b) a ## b
#define CAT2(a, b) CAT2_kernel(a, b)
#define CAT3_kernel(a, b, c) a ## b ## c
#define CAT3(a, b, c) CAT3_kernel(a, b, c)

/* Type defined by compiler flags in the macro TYPE */
/* Optionally, the list name new_[queue name]_list() for eg, is defined in NAME */

#ifndef NAME
#define NAME TYPE
#endif

#define LIST_T CAT2(NAME, _list_t)

typedef struct LIST_T LIST_T;

struct LIST_T
{
  TYPE* data;
  size_t size;
  size_t space;

};

LIST_T* CAT3(new_, NAME, _list)(void);
LIST_T* CAT2(NAME, _list_insert)(LIST_T* list, size_t index, TYPE data);
LIST_T* CAT2(NAME, _list_add)(LIST_T* list, TYPE data);
TYPE CAT2(NAME, _list_get)(LIST_T* list, size_t index);
TYPE CAT2(NAME, _list_remove)(LIST_T* list, size_t index);
char CAT2(NAME, _list_contains)(LIST_T* list, TYPE data, char (*equals)(TYPE a, TYPE b));
char CAT2(NAME, _list_is_empty)(LIST_T* list);
void CAT2(NAME, _list_clear)(LIST_T* list);
void CAT2(NAME, _list_clean)(LIST_T* list);
size_t CAT2(NAME, _list_size)(LIST_T* list);
