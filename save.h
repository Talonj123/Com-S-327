#ifndef _SAVE_H_
#define _SAVE_H

#include "dungeon/dungeons.hpp"

char check_make_dir();
char save_dungeon(const dungeon_t* dungeon, const char* name);
dungeon_t* load_dungeon(const char* name);

#endif //_SAVE_H_
