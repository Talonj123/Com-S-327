#ifndef _GAMEFLOW_H_
#define _GAMEFLOW_H_

#include "dungeon/dungeons.h"

/* all events must have these members first */
typedef struct event
{
  int time;
  /* pointer to self provided to allow the handler to work for multiple events (reducing duplicate code) */
  void (*run)(dungeon_t*, struct event* this_event);
  void (*cleanup)(dungeon_t*, struct event* this_event);
} event_t;

void do_next_event(dungeon_t* dungeon);
void add_event(event_t* event);
void clear_events(dungeon_t* dungeon);

void init_io();
void end_io();

void print_dungeon(dungeon_t* dungeon);

#endif // _GAMEFLOW_H_
