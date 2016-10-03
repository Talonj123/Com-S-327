 
#include "characters.h"
#include "gameflow.h"

typedef struct monster_event
{
  event_t base;
  monster_t* monster;

} monster_event_t;

monster_t* get_new_monster()
{
  monster_t* monster = malloc(sizeof(monster_t));
  char attributes = rand()%16;
  attributes = 0x08;
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

void monster_take_turn(dungeon_t* dungeon, event_t* this_event)
{
  monster_t* monster = ((monster_event_t*)this_event)->monster;

  if ((monster->attributes.erratic) && (rand() % 2))
  {
    while (1)
    {
      int dx = (rand() % 3) - 1;
      int dy = (rand() % 3) - 1;
      if (dx == 0 && dy == 0)
      {
	continue;
      }
      point_t loc = ((character_t*)monster)->loc;
      point_t nloc = {loc.x + dx, loc.y + dy};
      if (!(dungeon->terrain[nloc.y][nloc.x] == FLOOR ||
	   dungeon->terrain[nloc.y][nloc.x] == HALL))
      {
	continue;
      }
      if (dungeon->characters[nloc.y][nloc.x] != NULL)
      {
	character_t* character = dungeon->characters[nloc.y][nloc.x];
	if (character->type == PC)
	{
	  character->alive = 0;
	}
	else
	{
	  continue;
	}
      }
      dungeon->characters[loc.y][loc.x] = NULL;
      dungeon->characters[nloc.y][nloc.x] = (character_t*)monster;
      ((character_t*)monster)->loc = nloc;
      break;
    }
  } else
  {
    /* other attributes */
  }
  

  this_event->time += 100/((character_t*)monster)->speed;
  add_event((event_t*)this_event);
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
	monster_event_t* event = malloc(sizeof(monster_event_t));
	event->base.time = 100/((character_t*)monster)->speed;
	((event_t*)event)->run = monster_take_turn;
	event->monster = monster;
	add_event((event_t*)event);
	break;
      }
    }
  }
}
