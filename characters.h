#ifndef _CHARACTERS_H_
# define _CHARACTERS_H_

#include "dungeon/coordinates.h"

typedef enum {
  PC, MONSTER
} CharacterType;

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

#include "dungeon/dungeon_independant.h"

typedef struct
{
  tile_type terrain[DUNGEON_ROWS][DUNGEON_COLS];
} PlayerMemory;

class character_t
{
 public:
  char symbol;
  point_t loc;
  int speed;
  bool alive;
  CharacterType type;
};

class pc_t : public character_t
{
 public:
  character_t* target;
  PlayerMemory memory;

  void UpdateMemory(dungeon_t* dungeon);
};

class monster_t : public character_t
{
public:
  monster_attributes_t attributes;
  point_t last_known_pc;
};


typedef class character_t Character;
typedef class monster_t Monster;
typedef class pc_t Player;
  
#include <stdlib.h>
#include "dungeon/dungeons.h"

  monster_t* get_new_monster();
  pc_t* get_new_pc();

  void add_monsters(dungeon_t* dungeon, int num_monsters);
  void add_pc_event(pc_t* pc);
  char pc_try_move(dungeon_t* dungeon, pc_t* pc, int dx, int dy);

  void set_character_loc(character_t* pc, point_t point);

  point_t get_character_loc(character_t* pc);
  char get_character_symbol(character_t* character);
  CharacterType get_character_type(character_t* character);
  PlayerMemory get_pc_memory(pc_t* pc);
  void clear_pc_memory(pc_t* pc);

  void free_pc(pc_t* pc);
  void free_character(character_t* pc);

#endif //_CHARACTERS_H_
