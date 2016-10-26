#ifndef _DUNGEONS_H_
#define _DUNGEONS_H_

#include "coordinates.h"
#include "../characters.h"


typedef unsigned char hardness_t;

struct dungeon
{
  hardness_t hardness[DUNGEON_ROWS][DUNGEON_COLS];
  tile_type terrain[DUNGEON_ROWS][DUNGEON_COLS];
  int distance_to_pc[DUNGEON_ROWS][DUNGEON_COLS];
  int tunneling_distance_to_pc[DUNGEON_ROWS][DUNGEON_COLS];
  character_t* characters[DUNGEON_ROWS][DUNGEON_COLS];
  rectangle_t *rooms;
  int num_rooms;
  int num_characters;
  
  pc_t* pc;
  
};

//dungeon_t* get_blank_dungeon(int rows, int cols);
dungeon_t* dungeon_new();
void dungeon_free(dungeon_t* dungeon);
void set_pc(dungeon_t* dungeon, pc_t* pc);

#endif //_DUNGEONS_H_
