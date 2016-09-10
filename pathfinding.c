#include <limits.h>
#include <stdlib.h>

#include <stdio.h>

#include "dungeon/dungeons.h"

#define NAME tile
#define TYPE tile_t*
#define IS_POINTER 1
#include "data_structures/priority_queue.h"

void get_distances_non_tunnelling(dungeon_t* target);
void get_distances_tunnelling(dungeon_t* target);

void get_distances(dungeon_t* target)
{
  int r, c;
  for (r = 0; r < target->rows; r++)
  {
    for (c = 0; c < target->cols; c++)
    {
      target->tiles[r][c].distance_to_pc = INT_MAX;
      target->tiles[r][c].tunnelling_distance_to_pc = INT_MAX;
    }
  } 
  get_distances_non_tunnelling(target);
  get_distances_tunnelling(target);
}

int compare_tile_dist_non_tunnelling(tile_t* a, tile_t* b)
{
  return (a->distance_to_pc - b->distance_to_pc);
}

void visit_tile_non_tunnelling(tile_pqueue_t* pqueue, int cCost, tile_t* new_tile)
{
  if ((new_tile != NULL) && (new_tile->type != WALL))
  {
    if (new_tile->distance_to_pc > cCost + 1)
    {
      new_tile->distance_to_pc = cCost + 1;
      tile_pqueue_decrease_priority_add_ptr(pqueue, new_tile);
    }
  }
}

void get_distances_non_tunnelling(dungeon_t* target)
{
  tile_pqueue_t* pqueue = new_tile_pqueue(compare_tile_dist_non_tunnelling);
  point_t loc = target->pc.loc;
  tile_t* start = &target->tiles[loc.y][loc.x];
  start->distance_to_pc = 0;
  tile_pqueue_enqueue(pqueue, start);
  while (!tile_pqueue_is_empty(pqueue))
  {
    tile_t* visiting = tile_pqueue_dequeue(pqueue);
    visit_tile_non_tunnelling(pqueue, visiting->distance_to_pc, visiting->left);
    visit_tile_non_tunnelling(pqueue, visiting->distance_to_pc, visiting->right);
    visit_tile_non_tunnelling(pqueue, visiting->distance_to_pc, visiting->up);
    visit_tile_non_tunnelling(pqueue, visiting->distance_to_pc, visiting->down);
  }
  free(pqueue);
}

/**************************************************************************************/

int get_cost(tile_t* tile)
{
  if (tile->hardness == 0)
  {
    return 1;
  }
  else if (tile->hardness < 85)
  {
    return 2;
  }
  else if (tile->hardness < 171)
  {
    return 3;
  }
  else if (tile->hardness < 255)
  {
    return 4;
  }
  return 200;
}

int compare_tile_dist_tunnelling(tile_t* a, tile_t* b)
{
  return (a->tunnelling_distance_to_pc - b->tunnelling_distance_to_pc);
}

void visit_tile_tunnelling(tile_pqueue_t* pqueue, int cCost, tile_t* new_tile)
{
  if ((new_tile != NULL) && (new_tile->hardness < MAX_HARDNESS))
  {
    int new_cost = cCost + get_cost(new_tile);
    if (new_tile->tunnelling_distance_to_pc > new_cost)
    {
      new_tile->tunnelling_distance_to_pc = new_cost;
      tile_pqueue_decrease_priority_add_ptr(pqueue, new_tile);
    }
  }
}

void get_distances_tunnelling(dungeon_t* target)
{
  tile_pqueue_t* pqueue = new_tile_pqueue(compare_tile_dist_tunnelling);
  point_t loc = target->pc.loc;
  tile_t* start = &target->tiles[loc.y][loc.x];
  start->tunnelling_distance_to_pc = 0;
  tile_pqueue_enqueue(pqueue, start);
  while (!tile_pqueue_is_empty(pqueue))
  {
    tile_t* visiting = tile_pqueue_dequeue(pqueue);
    visit_tile_tunnelling(pqueue, visiting->tunnelling_distance_to_pc, visiting->left);
    visit_tile_tunnelling(pqueue, visiting->tunnelling_distance_to_pc, visiting->right);
    visit_tile_tunnelling(pqueue, visiting->tunnelling_distance_to_pc, visiting->up);
    visit_tile_tunnelling(pqueue, visiting->tunnelling_distance_to_pc, visiting->down);
  }
  free(pqueue);
}
