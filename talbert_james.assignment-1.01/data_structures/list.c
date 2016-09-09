#include <stdlib.h>
#include <string.h>
#include "list_internal.h"

LIST_T* CAT3(new_, NAME, _list)(void)
{
  LIST_T* list = malloc(sizeof(LIST_T));
  list->space = 5;
  list->data = malloc(sizeof(TYPE)*list->space);
  memset(list->data, 0, list->space);
  list->size = 0;
  return list;
}

LIST_T* CAT2(NAME, _list_insert)(LIST_T* list, size_t index, TYPE data)
{
  if (index > list->size)
  {
    /* complying would leave a gap in the list, non-sensical, so ignore the command */
    return list;
  }
  if (list->size == list->space)
  {
    /* double the space and copy items over */
      TYPE* new_data = malloc(sizeof(TYPE) * (list->space) * 2);
    memset(new_data, 0, list->space * 2);
    int i;
    for (i = 0; i < list->space; i++)
    {
      new_data[i] = list->data[i];
    }
    free(list->data);
    list->data = new_data;

    list->space *= 2;
  }
  /* shift everything (including data[index]) right by one */
  int i;
  for (i = list->size; i > index; i--)
  {
    list->data[i] = list->data[i-1];
  }
  /* add the data in data[index] */
  list->data[index] = data;
  list->size++;
  return list;
}

LIST_T* CAT2(NAME, _list_add)(LIST_T* list, TYPE data)
{
  CAT2(NAME, _list_insert)(list, list->size, data);
  return list;
}


TYPE CAT2(NAME, _list_get)(LIST_T* list, size_t index)
{
  TYPE to_return;
  if (index < list->size)
  {
    to_return = list->data[index];
  }
  return to_return;
}

TYPE CAT2(NAME, _list_remove)(LIST_T* list, size_t index)
{
  TYPE to_return;
  if (index < list->size)
  {
    to_return = list->data[index];
    list->size--;
    int i;
    for (i = index; i < list->size; i++)
    {
      list->data[i] = list->data[i+1];
    }
  }
  return to_return;
}

char CAT2(NAME, _list_contains)(LIST_T* list, TYPE data, char (*equals)(TYPE a, TYPE b))
{
  int i;
  for (i = 0; i < list->size; i++)
  {
    if (equals(data, list->data[i]))
    {
      return 1;
    }
  }
  return 0;
}

char CAT2(NAME, _list_is_empty)(LIST_T* list)
{
  return list->size == 0;
}

void CAT2(NAME, _list_clear)(LIST_T* list)
{
  list->size = 0;
}

void CAT2(NAME, _list_clean)(LIST_T* list)
{
  free(list->data);
}

size_t CAT2(NAME, _list_size)(LIST_T* list)
{
  return list->size;
}
