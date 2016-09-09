#include <stdlib.h>
#include "stack_internal.h"

struct STACK_NODE_T
{
  STACK_NODE_T* next;
  TYPE data;

};

struct STACK_T
{
  STACK_NODE_T* head;
  STACK_NODE_T* tail;

};

STACK_T* CAT3(new_, NAME, _stack)()
{
  STACK_T* stack = malloc(sizeof(STACK_T));
  stack->head = NULL;
  stack->tail = NULL;
  return stack;
}

STACK_T* CAT2(NAME, _stack_push)(STACK_T* stack, TYPE data)
{
  STACK_NODE_T* newNode = malloc(sizeof(STACK_NODE_T));
  newNode->next = stack->head;
  newNode->data = data;
  stack->head = newNode;
  return stack;
}

/* Removes the next item from the stack and returns it */
TYPE CAT2(NAME, _stack_pop)(STACK_T* stack)
{
  /* used to have something to return if the stack is empty */
  TYPE value;
  if (stack->head != NULL)
  {
    /* not empty */
    STACK_NODE_T* head = stack->head;
    stack->head = head->next;
    value = head->data;
free(head);
  }

  return value;
}

char CAT2(NAME, _stack_is_empty)(STACK_T* stack)
{
  return stack->head == NULL;
}


void CAT2(NAME, _stack_clear)(STACK_T* stack)
{
  while (stack->head != NULL)
  {
    STACK_NODE_T* head = stack->head;
    stack->head = head->next;
free(head);
  }
  stack->tail = NULL;
}
