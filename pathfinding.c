#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "dungeon/dungeons.h"

#define TYPE point_t
#define NAME point
#include "data_structures/priority_queue.h"

#define DISTANCE(point) dungeon->distance_to_pc[point.y][point.x]
#define TUNNELING(point) dungeon->tunneling_distance_to_pc[point.y][point.x]
#define TERRAIN(point) dungeon->terrain[point.y][point.x]
#define HARDNESS(point) dungeon->hardness[point.y][point.x]

void get_distances_non_tunneling(dungeon_t* target);
void get_distances_tunneling(dungeon_t* target);

/* The current dungeon being targeted */
dungeon_t* dungeon;

void get_distances(dungeon_t* target)
{
  dungeon = target;
  int r, c;
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      target->distance_to_pc[r][c] = INT_MAX-5;
      target->tunneling_distance_to_pc[r][c] = INT_MAX-5;
    }
  }
  get_distances_non_tunneling(target);
  get_distances_tunneling(target);
}

int compare_tile_dist_non_tunneling(point_t a, point_t b)
{
  return (DISTANCE(a) - DISTANCE(b));
}

void visit_tile_non_tunneling(dungeon_t* dungeon, point_pqueue_t* pqueue, int cCost, point_t new_tile)
{
  if (new_tile.x < 0 || new_tile.x > DUNGEON_COLS - 1 ||
      new_tile.y < 0 || new_tile.y > DUNGEON_ROWS - 1)
  {
    return;
  }
  if (dungeon->terrain[new_tile.y][new_tile.x] != WALL)
  {
    if (DISTANCE(new_tile) > cCost + 1)
    {
      DISTANCE(new_tile) = cCost + 1;
      point_pqueue_decrease_priority_add(pqueue, new_tile, point_equals);
    }
  }
}

void get_distances_non_tunneling(dungeon_t* dungeon)
{
  point_pqueue_t* pqueue = new_point_pqueue(compare_tile_dist_non_tunneling);
  point_t loc = get_character_loc((character_t*)dungeon->pc);

  DISTANCE(loc) = 0;
  point_pqueue_enqueue(pqueue, loc);
  while (!point_pqueue_is_empty(pqueue))
  {
    loc = point_pqueue_dequeue(pqueue);
    point_t l  = {loc.x - 1, loc.y};
    point_t ul = {loc.x - 1, loc.y - 1};
    point_t u  = {loc.x, loc.y - 1};
    point_t ur = {loc.x + 1, loc.y - 1};
    point_t r  = {loc.x + 1, loc.y};
    point_t dr = {loc.x + 1, loc.y + 1};
    point_t d  = {loc.x, loc.y + 1};
    point_t dl = {loc.x - 1, loc.y + 1};

    int distance = DISTANCE(loc);
    visit_tile_non_tunneling(dungeon, pqueue, distance, u);
    visit_tile_non_tunneling(dungeon, pqueue, distance, ur);
    visit_tile_non_tunneling(dungeon, pqueue, distance, r);
    visit_tile_non_tunneling(dungeon, pqueue, distance, dr);
    visit_tile_non_tunneling(dungeon, pqueue, distance, d);
    visit_tile_non_tunneling(dungeon, pqueue, distance, dl);
    visit_tile_non_tunneling(dungeon, pqueue, distance, l);
    visit_tile_non_tunneling(dungeon, pqueue, distance, ul);
  }
  point_pqueue_free(pqueue);
}

/**************************************************************************************/

int get_cost(dungeon_t* dungeon, point_t tile)
{
  hardness_t hardness = dungeon->hardness[tile.y][tile.x];
   if (hardness < 85)
  {
    return 1;
  }
  else if (hardness < 171)
  {
    return 2;
  }
  else if (hardness < 255)
  {
    return 3;
  }
  /* high enough that it won't ever be part of a path */
  /* shouldn't happen anyway */
  return 255;
}

int compare_tile_dist_tunneling(point_t a, point_t b)
{
  /* Added the hardness element to reduce the number of times that a tile gets re-added
   * and the whold ething re-evaluated. works because the tile that will produce the 
   * smallest values in other tiles get run first.
   */
  return (TUNNELING(a) - TUNNELING(b)) + (HARDNESS(a) - HARDNESS(b));
}

void visit_tile_tunneling(dungeon_t* dungeon, point_pqueue_t* pqueue, int cCost, point_t new_tile)
{
  if (new_tile.x < 0 || new_tile.x > DUNGEON_COLS - 1 ||
      new_tile.y < 0 || new_tile.y > DUNGEON_ROWS - 1)
  {
    return;
  }
  if (dungeon->hardness[new_tile.y][new_tile.x] < MAX_HARDNESS)
  {
    int new_cost = cCost + get_cost(dungeon, new_tile);
    if (TUNNELING(new_tile) > new_cost)
    {
      TUNNELING(new_tile) = new_cost;
      point_pqueue_decrease_priority_add(pqueue, new_tile, point_equals);
    }
  }
}

void get_distances_tunneling(dungeon_t* target)
{
  point_pqueue_t* pqueue = new_point_pqueue(compare_tile_dist_tunneling);
  point_t loc = get_character_loc((character_t*)dungeon->pc);
  TUNNELING(loc) = 0;
  point_pqueue_enqueue(pqueue, loc);
  while (!point_pqueue_is_empty(pqueue))
  {
    loc = point_pqueue_dequeue(pqueue);

    point_t l  = {loc.x -1, loc.y +0};
    point_t ul = {loc.x -1, loc.y -1};
    point_t u  = {loc.x +0, loc.y -1};
    point_t ur = {loc.x +1, loc.y -1};
    point_t r  = {loc.x +1, loc.y +0};
    point_t dr = {loc.x +1, loc.y +1};
    point_t d  = {loc.x +0, loc.y +1};
    point_t dl = {loc.x -1, loc.y +1};

    int distance = TUNNELING(loc);
    visit_tile_tunneling(dungeon, pqueue, distance, u);
    visit_tile_tunneling(dungeon, pqueue, distance, ur);
    visit_tile_tunneling(dungeon, pqueue, distance, r);
    visit_tile_tunneling(dungeon, pqueue, distance, dr);
    visit_tile_tunneling(dungeon, pqueue, distance, d);
    visit_tile_tunneling(dungeon, pqueue, distance, dl);
    visit_tile_tunneling(dungeon, pqueue, distance, l);
    visit_tile_tunneling(dungeon, pqueue, distance, ul);
  }
  point_pqueue_free(pqueue);
}
