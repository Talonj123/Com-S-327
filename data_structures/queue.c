#include <stdlib.h>
#include "queue_internal.h"

typedef struct QUEUE_NODE_T QUEUE_NODE_T;

struct QUEUE_NODE_T
{
  QUEUE_NODE_T* next;
  TYPE data;

};

struct QUEUE_T
{
  QUEUE_NODE_T* head;
  QUEUE_NODE_T* tail;

};

QUEUE_T* CAT3(new_, NAME, _queue)()
{
  QUEUE_T* queue = malloc(sizeof(QUEUE_T));
  queue->head = NULL;
  queue->tail = NULL;
  return queue;
}

QUEUE_T* CAT2(NAME, _queue_enqueue)(QUEUE_T* queue, TYPE data)
{
  QUEUE_NODE_T* newNode = malloc(sizeof(QUEUE_NODE_T));
  newNode->next = NULL;
  newNode->data = data;
  if (queue->head != NULL)
  {
    /* not empty */
    queue->tail->next = newNode;
    queue->tail = newNode;
  }
  else
  {
    /* empty */ 
    queue->head = newNode;
    queue->tail = newNode;
  }
  return queue;
}

/* Removes the next item from the queue and returns it */
TYPE CAT2(NAME, _queue_dequeue)(QUEUE_T* queue)
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

char CAT2(NAME, _queue_is_empty)(QUEUE_T* queue)
{
  return queue->head == NULL;
}


void CAT2(NAME, _queue_clear)(QUEUE_T* queue)
{
  while (queue->head != NULL)
  {
    QUEUE_NODE_T* head = queue->head;
    queue->head = head->next;
    free(head);
  }
  queue->tail = NULL;
}

void CAT2(NAME, _queue_free)(QUEUE_T* queue)
{
  while (queue->head != NULL)
  {
    QUEUE_NODE_T* head = queue->head;
    queue->head = head->next;
    free(head);
  }
  queue->tail = NULL;
  free(queue);
}
