#ifndef _CHARACTERS_H_
# define _CHARACTERS_H_

typedef struct character character_t;
typedef struct pc pc_t;

#include "dungeon/coordinates.h"
#include "dungeon/dungeons.h"

#include <stdlib.h>

typedef union monster_attributes
{
  unsigned char raw : 4;
  struct
  {
    unsigned char intelligent : 1;
    unsigned char telepathic : 1;
    unsigned char tunneling : 1;
    unsigned char erratic : 1;
  };
} monster_attributes_t;

typedef enum character_type
{
  PC, MONSTER
    
} character_type;

struct character
{
  point_t loc;
  char symbol;
  int speed;
  char alive;

  character_type type;

};

struct pc
{
  character_t base;

};

typedef struct monster
{
  character_t base;
  monster_attributes_t attributes;

  /* last los  position for non-telepathic and current position for telepathic */
  point_t last_pc_known;

} monster_t;


monster_t* get_new_monster();
pc_t* get_new_pc();

void add_monsters(dungeon_t* dungeon, int num_monsters);

#endif //_CHARACTERS_H_
