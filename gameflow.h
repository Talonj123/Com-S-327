#ifndef _GAMEFLOW_H_
#define _GAMEFLOW_H_

#include "dungeon/dungeons.h"

typedef struct game_state
{
  char running : 1;
  char quitted : 1;
  char reload : 1;
} game_state_t;

extern game_state_t game_state;

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

#endif // _GAMEFLOW_H_
