#ifndef _IO_H_
# define _IO_H_

#include "characters.h"
#include "dungeons.hpp"

void pc_turn_interface(dungeon_t* dungeon, player* pc);
void print_dungeon(dungeon_t* dungeon);
void init_io();
void end_io();

#endif //_IO_H_
