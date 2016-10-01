#ifndef _DUNGEONS_H_
#define _DUNGEONS_H_

#include "coordinates.h"
#include "../characters.h"

#define FLOOR_HARDNESS 0
#define HALL_HARDNESS 0
#define MAX_HARDNESS (hardness_t)255
#define MIN_HARDNESS (hardness_t)10

#define DUNGEON_ROWS 21
#define DUNGEON_COLS 80

typedef unsigned char hardness_t;

typedef enum tile_type
{
  FLOOR, HALL, WALL
} tile_type;

typedef struct dungeon_t
{
hardness_t hardness[DUNGEON_ROWS][DUNGEON_COLS];
tile_type terrain[DUNGEON_ROWS][DUNGEON_COLS];
int distance_to_pc[DUNGEON_ROWS][DUNGEON_COLS];
int tunneling_distance_to_pc[DUNGEON_ROWS][DUNGEON_COLS];
rectangle_t *rooms;
int num_rooms;

pc_t pc;

} dungeon_t;

//dungeon_t* get_blank_dungeon(int rows, int cols);
dungeon_t* dungeon_new();
void dungeon_free(dungeon_t* dungeon);

#endif //_DUNGEONS_H_
