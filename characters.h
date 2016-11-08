#ifndef _CHARACTERS_H_
# define _CHARACTERS_H_
#include "dungeon/coordinates.h"

#include <vector>

typedef enum {
  PC, MONSTER
} CharacterType;

typedef union monster_attributes
{
  unsigned char raw : 5;
  struct
  {
    unsigned char intelligent : 1;
    unsigned char telepathic : 1;
    unsigned char tunneling : 1;
    unsigned char erratic : 1;
    unsigned char corporeal : 1;
  };
} monster_attributes_t;

#include "dungeon/dungeon_independant.h"

typedef struct
{
  tile_type terrain[DUNGEON_ROWS][DUNGEON_COLS];
} PlayerMemory;

class character
{
 public:
  char symbol;
  std::vector<int> colors;
  point_t loc;
  int speed;
  bool alive;
  CharacterType type;
};

class player : public character
{
 public:
  character* target;
  PlayerMemory memory;

  void UpdateMemory(dungeon_t* dungeon);
};

#include "dice.hpp"

class monster : public character
{
public:
  monster();
  monster_attributes_t attributes;
  point_t last_known_pc;
  dice damage;
};

#include <stdlib.h>
#include "dungeon/dungeons.h"
#include "generation.hpp"

  monster* get_new_monster();
  player* get_new_pc();

void add_monsters(dungeon_t* dungeon, std::vector<monster_data> types, int num_monsters);
  void add_pc_event(player* pc);
  char pc_try_move(dungeon* dungeon, player* pc, int dx, int dy);

  void set_character_loc(character* pc, point_t point);

  point_t get_character_loc(character* character);
  char get_character_symbol(character* character);
  CharacterType get_character_type(character* character);
  PlayerMemory get_pc_memory(player* pc);
  void clear_pc_memory(player* pc);

  void free_pc(player* pc);
  void free_character(character* pc);

#endif //_CHARACTERS_H_




































































































































































