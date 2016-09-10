#ifndef _DUNGEONS_H_
#define _DUNGEONS_H_

#include "coordinates.h"

#define FLOOR_HARDNESS 0
#define HALL_HARDNESS 0
#define MAX_HARDNESS (hardness_t)255
#define MIN_HARDNESS (hardness_t)10

typedef int hardness_t;

typedef enum tile_type
{
  FLOOR, HALL, WALL
} tile_type;

typedef struct tile_t tile_t;
struct tile_t
{
  tile_type type;
  point_t loc;

  /* tiles with a high hardness are harder to dig/mine/break */
  hardness_t hardness;
  char is_room_center;

  tile_t *up, *down, *right, *left;

  int distance_to_pc;
  int tunnelling_distance_to_pc;
};

typedef struct player_t
{
  point_t loc;

} player_t;

typedef struct dungeon_t
{
  int rows, cols;
  tile_t** tiles;
  player_t pc;

} dungeon_t;

dungeon_t* get_blank_dungeon(int rows, int cols);
dungeon_t* get_dungeon(int rows, int cols, char rectangular);

#endif //_DUNGEONS_H_
