 
#include "characters.h"

monster_t* get_new_monster()
{
  monster_t* monster = malloc(sizeof(monster_t));
  char attributes = rand()%16;
  int speed = rand()%15 + 5;
  monster->attributes.raw = attributes;
  if (attributes < 0xA)
  {
    ((character_t*)monster)->symbol = '0' + attributes;
  } else
  {
    ((character_t*)monster)->symbol = 'A' + attributes - 0xA;
  }
  ((character_t*)monster)->speed = speed;
  ((character_t*)monster)->alive = 1;
  return monster;
}

pc_t* get_new_pc()
{
  pc_t* pc = malloc(sizeof(pc_t));;
  ((character_t*)pc)->symbol = '@';
  ((character_t*)pc)->speed = 10;
  ((character_t*)pc)->alive = 1;
  return pc;
}

void add_monsters(dungeon_t* dungeon, int num_monsters)
{
  int i;
  for (i = 0; i < num_monsters; i++)
  {
    monster_t* monster = get_new_monster();
    while (1)
    {
      int r = rand() % DUNGEON_ROWS;
      int c = rand() % DUNGEON_COLS;
      if (dungeon->terrain[r][c] == FLOOR &&
	  dungeon->characters[r][c] == NULL)
      {
	((character_t*)monster)->loc.x = c;
	((character_t*)monster)->loc.y = r;
	dungeon->characters[r][c] = (character_t*)monster;
	break;
      }
    }
  }
}
