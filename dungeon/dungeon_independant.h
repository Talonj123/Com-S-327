
#ifndef _DUNGEON_INDEPENDANT_H_
#define _DUNGEON_INDEPENDANT_H_

typedef enum tile_type
{
  FLOOR, HALL, WALL, UP_STAIR, DOWN_STAIR
} tile_type;

typedef struct dungeon dungeon_t;

#define FLOOR_HARDNESS 0
#define HALL_HARDNESS 0
#define MAX_HARDNESS (hardness_t)255
#define MIN_HARDNESS (hardness_t)10

#define DUNGEON_ROWS 21
#define DUNGEON_COLS 80

#endif //_DUNGEON_INDEPENDANT_H_
