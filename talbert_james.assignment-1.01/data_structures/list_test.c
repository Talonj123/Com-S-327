#include <stdio.h>

#define TYPE int
#include "list.h"


int main(int argc, char *argv[])
{
  int i;
  int_list_t* list = new_int_list();
  printf("List created\n");
  int_list_add(list, 1);
  int_list_add(list, 3);
  int_list_add(list, 4);
  int_list_add(list, 5);
  printf("Added 4 items\n");

  
  for (i = 0; i < int_list_size(list); i++)
  {
    printf("%d\n", int_list_get(list, i));
  }

  int_list_insert(list, 1, 2);
  printf("Inserted 5th\n");

  
  for (i = 0; i < int_list_size(list); i++)
  {
    printf("%d\n", int_list_get(list, i));
  }

  int_list_remove(list, 2);

  printf("Size %d:\n", (int)int_list_size(list));
  for (i = 0; i < int_list_size(list); i++)
  {
    printf("%d\n", int_list_get(list, i));
  }

  return 0;
}
