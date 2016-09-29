 
#include "characters.h"

monster_t  get_new_monster()
{
  monster_t monster;
  char attributes = rand()%16;
  int speed = rand()%15 + 5;
  monster.attributes.raw = attributes;
  if (attributes < 0xA)
  {
    ((character_t*)&monster)->symbol = '0' + attributes;
  } else
  {
    ((character_t*)&monster)->symbol = 'A' + attributes - 0xA;
  }
  ((character_t*)&monster)->speed = speed;
  ((character_t*)&monster)->alive = 1;
  return monster;
}

pc_t get_new_pc()
{
  pc_t pc;
  ((character_t*)&pc)->symbol = '@';
  ((character_t*)&pc)->speed = 10;
  ((character_t*)&pc)->alive = 1;
  return pc;
}
