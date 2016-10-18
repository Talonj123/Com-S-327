 
#include "gameflow.h"

#define TYPE event_t*
#define NAME event
#define IS_POINTER 1
#include "data_structures/priority_queue.h"

#include <stdio.h>
#include <ncurses.h>

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

void clear_events(dungeon_t* dungeon)
{
  if (events == NULL)
  {
    return;
  }
  while (!event_pqueue_is_empty(events))
  {
    event_t* event = event_pqueue_dequeue(events);
    event->cleanup(dungeon, event);
  }
  event_pqueue_free(events);
  events = NULL;
}

void init_io()
{
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
}

void end_io()
{
  endwin();
}

void print_dungeon(dungeon_t* dungeon)
{
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_type tile = dungeon->terrain[r][c];
      char ch = ' ';
      if (dungeon->characters[r][c] != NULL)
      {
        ch = dungeon->characters[r][c]->symbol;
      }
      else if (tile == WALL)
      {
	ch = ' ';
      }
      else if (tile == FLOOR)
      {
        ch = '.';
      }
      else if (tile == HALL)
      {
        ch = '#';
      }
      mvaddch(r, c, ch);
    }
  }
  refresh();
}
