 
#include "gameflow.h"
#include "pqueue.hpp"

#include <stdio.h>
#include <ncurses.h>

bool compare_events(event_t* a, event_t* b);

PriorityQueue<event_t*> events(compare_events);

bool compare_events(event_t* a, event_t* b)
{
  return a->time > b->time;
}

void do_next_event(dungeon_t* dungeon)
{
  if (events.empty())
  {
    return;
  }
  event_t* event = events.dequeue();
  event->run(dungeon, event);
}

void add_event(event_t* event)
{
  events.enqueue(event);
}

void clear_events(dungeon_t* dungeon)
{
  while (!events.empty())
  {
    event_t* event = events.dequeue();
    event->cleanup(dungeon, event);
  }
}
