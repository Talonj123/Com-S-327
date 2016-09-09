#include <stdlib.h>
#include "priority_queue_internal.h"

typedef struct QUEUE_NODE_T QUEUE_NODE_T;

struct QUEUE_NODE_T
{
  QUEUE_NODE_T* next;
  TYPE data;

};

/* The queue wrapper  */
struct QUEUE_T
{
  QUEUE_NODE_T* head;
  QUEUE_NODE_T* tail;

  int (*compare)(TYPE a, TYPE b);

};

QUEUE_T* CAT3(new_, NAME, _pqueue)(int (*compare)(TYPE a, TYPE b))
{
  QUEUE_T* queue = malloc(sizeof(QUEUE_T));
  queue->head = NULL;
  queue->tail = NULL;
  queue->compare = compare;
  return queue;
}

QUEUE_T* CAT2(NAME, _pqueue_enqueue)(QUEUE_T* queue, TYPE data)
{
  QUEUE_NODE_T* newNode = malloc(sizeof(QUEUE_NODE_T));
  newNode->next = NULL;
  newNode->data = data;
  if (queue->head != NULL)
  {
    /* not empty */
    if (queue->compare(data, queue->head->data) < 0)
    {
      newNode->next = queue->head;
      queue->head = newNode;
      queue->tail->next = NULL;
      return queue;
    }
    /* if it gets to here, then the new node goes after the head at some point */
    QUEUE_NODE_T* cNode = queue->head;
    QUEUE_NODE_T* pNode = queue->head;
    char inserted = 0;
    while (cNode != NULL)
    {
      if (queue->compare(data, cNode->data) < 0)
      { /* insert in-between cNode and pNode */
	pNode->next = newNode;
	newNode->next = cNode;
	inserted = 1;
	break;
      }
      pNode = cNode;
      cNode = cNode->next;
    }
    if (inserted == 0)
    {
      pNode->next = newNode;
      queue->tail = newNode;
    }
  }
  else
  {
    /* empty */ 
    queue->head = newNode;
    queue->tail = newNode;
  }
  queue->tail->next = NULL;
  return queue;
}

/* Removes the next item from the queue and returns it */
TYPE CAT2(NAME, _pqueue_dequeue)(QUEUE_T* queue)
{
  /* used to have something to return if the queue is empty */
  TYPE value;
  if (queue->head != NULL)
  {
    /* not empty */
    QUEUE_NODE_T* head = queue->head;
    queue->head = head->next;
    value = head->data;
free(head);
  }

  return value;
}

QUEUE_T* CAT2(NAME, _pqueue_update_priority)(QUEUE_T* queue, TYPE data, char (*equals)(TYPE a, TYPE b))
{
  if ((queue->head != NULL) && equals(queue->head->data, data))
  {
    QUEUE_NODE_T* node = queue->head;
    queue->head = node->next;
    CAT2(NAME, _pqueue_enqueue)(queue, node->data);
free(node);
  }
  QUEUE_NODE_T* pNode = queue->head;
  QUEUE_NODE_T* cNode = queue->head;
  while (cNode != NULL)
  {
    if (equals(cNode->data, data))
    {
      pNode->next = cNode->next;
      CAT2(NAME, _pqueue_enqueue)(queue, cNode->data);
free(cNode);
    }
  }
  return queue;
}

char CAT2(NAME, _pqueue_is_empty)(QUEUE_T* queue)
{
  return queue->head == NULL;
}


void CAT2(NAME, _pqueue_clear)(QUEUE_T* queue)
{
  while (queue->head != NULL)
  {
    QUEUE_NODE_T* head = queue->head;
    queue->head = head->next;
free(head);
  }
  queue->tail = NULL;
}
