#ifndef _DUNGEONS_PRIVATE_H_
#define _DUNGEONS_PRIVATE_H_

#include "dungeons.hpp"

typedef struct tile_dijkstra_t tile_dijkstra_t;
struct tile_dijkstra_t
{
  point_t tile;
  int total_cost;
  tile_dijkstra_t* parent;
  int manhattan;

};

#endif //_DUNGEONS_PRIVATE_H_
