#include <limits.h>
#include <stdlib.h>

#include <stdio.h>

#include "dungeon/dungeons.h"

#define NAME tile
#define TYPE tile_t*
#define IS_POINTER 1
#include "data_structures/priority_queue.h"

int compare_tile_dist_non_tunneling(tile_t* a, tile_t* b)
{
  return (a->distance_to_pc - b->distance_to_pc);
}

void visit_tile_non_tunneling(tile_pqueue_t* pqueue, int cCost, tile_t* new_tile, tile_t* parent)
{
  if ((new_tile != NULL) && (new_tile->type != WALL))
  {
    if (new_tile->distance_to_pc > cCost + 1)
    {
      new_tile->distance_to_pc = cCost + 1;
      tile_pqueue_decrease_priority_add_ptr(pqueue, new_tile);
    }
  }
  else if (new_tile == NULL)
  {
    printf("Found NULL tile (%d, %d)\n", parent->loc.x, parent->loc.y);
  }
}

void get_distances(dungeon_t* target)
{
  int r, c;
  for (r = 0; r < target->rows; r++)
  {
    for (c = 0; c < target->cols; c++)
    {
      target->tiles[r][c].distance_to_pc = INT_MAX;
    }
  } 
  tile_pqueue_t* pqueue = new_tile_pqueue(compare_tile_dist_non_tunneling);
  point_t loc = target->pc.loc;
  tile_t* start = &target->tiles[loc.y][loc.x];
  start->distance_to_pc = 0;
  tile_pqueue_enqueue(pqueue, start);
  while (!tile_pqueue_is_empty(pqueue))
  {
    tile_t* visiting = tile_pqueue_dequeue(pqueue);
    visit_tile_non_tunneling(pqueue, visiting->distance_to_pc, visiting->left, visiting);
    visit_tile_non_tunneling(pqueue, visiting->distance_to_pc, visiting->right, visiting);
    visit_tile_non_tunneling(pqueue, visiting->distance_to_pc, visiting->up, visiting);
    visit_tile_non_tunneling(pqueue, visiting->distance_to_pc, visiting->down, visiting);
  }
  free(pqueue);
}
