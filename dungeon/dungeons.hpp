#ifndef _DUNGEONS_H_
#define _DUNGEONS_H_

#include "coordinates.h"
#include "../characters.h"
#include "dungeon_independant.h"
#include "../items.hpp"

typedef unsigned char hardness_t;

class dungeon
{
  friend void init_dungeon(dungeon_t*);
private:  
  player* pc;

public:
  hardness_t hardness[DUNGEON_ROWS][DUNGEON_COLS];
  tile_type terrain[DUNGEON_ROWS][DUNGEON_COLS];
  int distance_to_pc[DUNGEON_ROWS][DUNGEON_COLS];
  int tunneling_distance_to_pc[DUNGEON_ROWS][DUNGEON_COLS];
  character* characters[DUNGEON_ROWS][DUNGEON_COLS];
  item* items[DUNGEON_ROWS][DUNGEON_COLS];
  rectangle_t *rooms;
  int num_rooms;
  int num_characters;
  
  void set_pc(player* pc);
  player* get_pc();

  dungeon(bool initMap = false);
  ~dungeon();
};

#endif //_DUNGEONS_H_
