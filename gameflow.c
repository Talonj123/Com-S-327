 
#include "gameflow.h"

#define TYPE event_t*
#define NAME event
#define IS_POINTER 1
#include "data_structures/priority_queue.h"

#include <stdio.h>

event_pqueue_t* events = NULL;
int compare_events(event_t* a, event_t* b)
{
  return a->time - b->time;
}

void do_next_event(dungeon_t* dungeon)
{
  if (events == NULL || event_pqueue_is_empty(events))
  {
    return;
  }
  event_t* event = event_pqueue_dequeue(events);
  event->run(dungeon, event);
}

void add_event(event_t* event)
{
  if (events == NULL)
  {
    events = new_event_pqueue(compare_events);
  }
  event_pqueue_enqueue(events, event);
}

void clear_events()
{
  if (events == NULL)
  {
    return;
  }
  while (!event_pqueue_is_empty(events))
  {
    free(event_pqueue_dequeue(events));
  }
  event_pqueue_free(events);
  events = NULL;
}

void print_dungeon(dungeon_t* dungeon)
{
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_type tile = dungeon->terrain[r][c];
      if (dungeon->characters[r][c] != NULL)
      {
	printf("%c", dungeon->characters[r][c]->symbol);
      }
      else if (tile == WALL)
      {
	printf("%c", ' ');
      }
      else if (tile == FLOOR)
      {
        printf("%c", '.');
      }
      else if (tile == HALL)
      {
        printf("%c", '#');
      }
    }
    printf("\n");
  }
}

void print_types(dungeon_t* dungeon)
{
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      if (dungeon->terrain[r][c] == WALL)
      {
	printf("%c", 'W');
      }
      else if (dungeon->terrain[r][c] == FLOOR)
      {
        printf("%c", 'F');
      }
      else if (dungeon->terrain[r][c] == HALL)
      {
        printf("%c", 'H');
      }
    }
    printf("\n");
  }
}

void print_distances_non_tunneling(dungeon_t* dungeon)
{
  char distance_markers[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_type tile = dungeon->terrain[r][c];
      if (dungeon->distance_to_pc[r][c] < sizeof(distance_markers)-1)
      {
	printf("%c", distance_markers[dungeon->distance_to_pc[r][c]]);
      }
      else if (tile == WALL)
      {
	printf("%c", ' ');
      }
      else if (tile == FLOOR)
      {
        printf("%c", '.');
      }
      else if (tile == HALL)
      {
        printf("%c", '#');
      }
    }
    printf("\n");
  }
}


void print_distances_tunneling(dungeon_t* dungeon)
{
  char distance_markers[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_type tile = dungeon->terrain[r][c];
      if (dungeon->tunneling_distance_to_pc[r][c] < sizeof(distance_markers)-1)
      {
	printf("%c", distance_markers[dungeon->tunneling_distance_to_pc[r][c]]);
      }
      else if (tile == WALL)
      {
	printf("%c", ' ');
      }
      else if (tile == FLOOR)
      {
        printf("%c", '.');
      }
      else if (tile == HALL)
      {
        printf("%c", '#');
      }
    }
    printf("\n");
  }
}
