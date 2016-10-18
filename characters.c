#include "characters.h"
#include "gameflow.h"
#include "pathfinding.h"

#include <limits.h>
#include <unistd.h>
#include <stdio.h>

#include <ncurses.h>

#define ERRATIC (0x08)
#define TUNNELING (0x04)
#define TELEPATHIC (0x02)
#define INTELLIGENT (0x01)

typedef struct monster_event
{
  event_t base;
  monster_t* monster;

} monster_event_t;

typedef struct pc_event
{
  event_t base;
  pc_t* pc;

} pc_event_t;

monster_t* get_new_monster()
{
  monster_t* monster = malloc(sizeof(monster_t));
  char attributes = rand()%16;
  int speed = rand()%15 + 5;
  //attributes = 10;
  monster->attributes.raw = attributes;
  if (attributes < 0xA)
  {
    ((character_t*)monster)->symbol = '0' + attributes;
  } else
  {
    ((character_t*)monster)->symbol = 'A' + attributes - 0xA;
  }
  ((character_t*)monster)->type = MONSTER;
  ((character_t*)monster)->speed = speed;
  ((character_t*)monster)->alive = 1;
  monster->last_pc_known.x = 0;
  monster->last_pc_known.y = 0;
  return monster;
}

pc_t* get_new_pc()
{
  pc_t* pc = malloc(sizeof(pc_t));
  ((character_t*)pc)->symbol = '@';
  ((character_t*)pc)->speed = 10;
  ((character_t*)pc)->type = PC;
  ((character_t*)pc)->alive = 1;
  pc->target = NULL;
  return pc;
}

void move_character(dungeon_t* dungeon, character_t* character, point_t nloc, char tunneling)
{
  if (dungeon->terrain[nloc.y][nloc.x] == WALL)
  {
    if (!tunneling || (dungeon->hardness[nloc.y][nloc.x] == MAX_HARDNESS))
    {
      /* can't move there */
      return;
    }
    
    if (dungeon->hardness[nloc.y][nloc.x] == MAX_HARDNESS)
    {
      return;
    }
    else if (dungeon->hardness[nloc.y][nloc.x] > 85)
    {
      dungeon->hardness[nloc.y][nloc.x]-= 85;
      get_distances(dungeon);
      return;
    }
    else
    {
      dungeon->hardness[nloc.y][nloc.x] = 0;
      dungeon->terrain[nloc.y][nloc.x] = HALL;
      get_distances(dungeon);
    }
  }
  point_t loc = character->loc;

  if (dungeon->characters[nloc.y][nloc.x] != NULL)
  {
    character_t* character = dungeon->characters[nloc.y][nloc.x];
    character->alive = 0;
    dungeon->characters[nloc.y][nloc.x] = NULL;
    /* let the event handler clean it up */
    dungeon->num_characters--;
  }
  dungeon->characters[nloc.y][nloc.x] = character;
  dungeon->characters[loc.y][loc.x] = NULL;
  character->loc = nloc;
}

char character_try_move_random(dungeon_t* dungeon, character_t* character, char tunneling)
{
  int dx = (rand() % 3) - 1;
  int dy = (rand() % 3) - 1;
  if (dx == 0 && dy == 0)
  {
    return 0;
  }
  point_t loc = character->loc;
  point_t nloc = loc;
  nloc.x += dx;
  nloc.y += dy;
  if (dungeon->terrain[nloc.y][nloc.x] == WALL && !tunneling)
  {
    return 0;
  }
  move_character(dungeon, character, nloc, tunneling);
  return 1;
}

char has_los_to_pc(dungeon_t* dungeon, monster_t* monster)
{
  int room_num;
  for (room_num = 0; room_num < dungeon->num_rooms; room_num++)
  {
    rectangle_t room = dungeon->rooms[room_num];
    if (rect_contains_point(room, ((character_t*)dungeon->pc)->loc)
	&& rect_contains_point(room, ((character_t*)monster)->loc))
    {
      return 1;
    }
  }
  return 0;
}


void move_toward_pc_path(dungeon_t* dungeon, monster_t* monster)
{
  int (*distance_map)[DUNGEON_ROWS][DUNGEON_COLS] = NULL;
  if (monster->attributes.tunneling && monster->attributes.intelligent)
  {
    /* use tunneling map */
    distance_map = &dungeon->tunneling_distance_to_pc;
  } else if (monster->attributes.intelligent)
  {
    /* use non-tunneling map */
    distance_map = &dungeon->distance_to_pc;
  }
  /* move according to distance map */
  point_t loc = ((character_t*)monster)->loc;
  int lowest_score = INT_MAX;
  point_t nloc = loc;
  int dx, dy;
  for (dx = -1; dx <= 1; dx++)
  {
    for (dy = -1; dy <= 1; dy++)
    {
      int score = (*distance_map)[loc.y + dy][loc.x + dx];
      if (score < lowest_score)
      {
	lowest_score = score;
	nloc.x = loc.x + dx;
	nloc.y = loc.y + dy;
      }
    }
  }
  move_character(dungeon, (character_t*)monster, nloc, monster->attributes.tunneling);
}

void move_toward_pc_line(dungeon_t* dungeon, monster_t* monster)
{
  point_t pcloc = monster->last_pc_known;
  point_t monloc = ((character_t*)monster)->loc;
  point_t delta = {pcloc.x - monloc.x, pcloc.y - monloc.y};
  
  int dx = 0, dy = 0;
  if (delta.x > 0)
  {
    dx = 1;
  } else if (delta.x < 0)
  {
    dx = -1;
  }
  if (delta.y > 0)
  {
    dy = 1;
  } else if (delta.y < 0)
  {
    dy = -1;
  }
  point_t nloc = {monloc.x + dx, monloc.y + dy};
  if (!monster->attributes.tunneling)
  {
    if (dungeon->terrain[nloc.y][nloc.x] == WALL)
    {
      if (dungeon->terrain[monloc.y][monloc.x + dx] != WALL)
      {
	nloc.y = monloc.y;
      }
      else if (dungeon->terrain[monloc.y + dy][monloc.x] != WALL)
      {
	nloc.x = monloc.x;
      }
      else
      {
	return;
      }
    }
  }
  move_character(dungeon, (character_t*)monster, nloc, monster->attributes.tunneling);
}

void monster_take_turn(dungeon_t* dungeon, event_t* this_event)
{
  monster_t* monster = ((monster_event_t*)this_event)->monster;
  if (!((character_t*)monster)->alive)
  {
    /* delete monster and  don't re-add event */
    /* removed from character map already when killed */
    free(monster);
    free(this_event);
    return;
  }

  if (monster->attributes.telepathic)
  {
    monster->last_pc_known = ((character_t*)dungeon->pc)->loc;
  }
  else if (has_los_to_pc(dungeon, monster))
  {
    monster->last_pc_known = ((character_t*)dungeon->pc)->loc;
  }
  else if (!monster->attributes.intelligent)
  {
    monster->last_pc_known.x = 0;
    monster->last_pc_known.y = 0;
  }

  if (monster->attributes.erratic && (rand() % 2))
  {
    int tries = 0;
    while (tries++ < 5)
    {
      if (character_try_move_random(dungeon, (character_t*)monster, monster->attributes.tunneling))
	break;
    }
  } else
  {
    /* other cases */
    if (monster->attributes.telepathic && monster->attributes.intelligent)
    {
      move_toward_pc_path(dungeon, monster);
    }
    else if (monster->last_pc_known.x > 0 &&
	     monster->last_pc_known.x < DUNGEON_COLS - 1 &&
	     monster->last_pc_known.y > 0 &&
	     monster->last_pc_known.y < DUNGEON_ROWS - 1)
    {
      move_toward_pc_line(dungeon, monster);
    }
    else
    {
      character_try_move_random(dungeon, (character_t*)monster, monster->attributes.tunneling);
    }
  }

  this_event->time += 100/((character_t*)monster)->speed;
  add_event((event_t*)this_event);
}

char pc_try_move(dungeon_t* dungeon, pc_t* pc, int dx, int dy)
{
  char tunneling = 0;
  point_t loc = ((character_t*)pc)->loc;
  point_t nloc = { loc.x + dx, loc.y + dy };

  if (dungeon->terrain[nloc.y][nloc.x] == WALL)
  {
    if (!tunneling || (dungeon->hardness[nloc.y][nloc.x] == MAX_HARDNESS))
    {
      /* can't move there */
      return 0;
    }
    if (dungeon->hardness[nloc.y][nloc.x] == MAX_HARDNESS)
    {
      return 0;
    }
    else if (dungeon->hardness[nloc.y][nloc.x] > 85)
    {
      dungeon->hardness[nloc.y][nloc.x]-= 85;
      get_distances(dungeon);
      return 1;
    }
    else
    {
      dungeon->hardness[nloc.y][nloc.x] = 0;
      dungeon->terrain[nloc.y][nloc.x] = HALL;
      get_distances(dungeon);
    }
  }

  if (dungeon->characters[nloc.y][nloc.x] != NULL)
  {
    character_t* character = dungeon->characters[nloc.y][nloc.x];
    character->alive = 0;
    dungeon->characters[nloc.y][nloc.x] = NULL;
    /* let the event handler clean it up */
    dungeon->num_characters--;
  }
  dungeon->characters[nloc.y][nloc.x] = (character_t*)pc;
  dungeon->characters[loc.y][loc.x] = NULL;
  ((character_t*)pc)->loc = nloc;
  return 1;
}

void pc_take_turn(dungeon_t* dungeon, event_t* this_event)
{
  pc_t* pc = ((pc_event_t*)this_event)->pc;
  if (!((character_t*)pc)->alive)
  {
    free(this_event);
    return;
  }
  
  //point_t loc = ((character_t*)pc)->loc;

  int ch = getch();
  switch (ch)
  {
  default:
    mvprintw(0, 0, "Unknown Key: %#o  ", ch);
    break;

    /* numpad.7 (up+left) */
  case 067:
    /* try move up+left */
    pc_try_move(dungeon, pc, -1, -1);
    break;

    /* numpad.8 (up) */
    case 070:
    /* try move up */
    pc_try_move(dungeon, pc, 0, -1);
    break;

    /* numpad.9 (up+right) */
    case 071:
    /* try move up+right */
    pc_try_move(dungeon, pc, 1, -1);
    break;

    /* numpad.1 (down+left) */
    case 061:
    /* try move down+left */
    pc_try_move(dungeon, pc, -1, 1);
    break;

    /* numpad.2 (down) */
    case 062:
    /* try move down */
    pc_try_move(dungeon, pc, 0, 1);
    break;

    /* numpad.3 (down+right) */
    case 063:
    /* try move down+left */
    pc_try_move(dungeon, pc, 1, 1);
    break;

    /* numpad.4 (left) */
    case 064:
    /* try move left */
    pc_try_move(dungeon, pc, -1, 0);
    break;

    /* numpad.4 (left) */
    case 066:
    /* try move left */
    pc_try_move(dungeon, pc, 1, 0);
    break;
  }

  print_dungeon(dungeon);
  this_event->time += 100/((character_t*)pc)->speed;
  add_event(this_event);
}

void monster_cleanup(dungeon_t* dungeon, event_t* this_event)
{
  monster_event_t* monster_event = ((monster_event_t*)this_event);
  monster_t* monster = monster_event->monster;
  point_t loc = ((character_t*)monster)->loc;
  dungeon->characters[loc.y][loc.x] = NULL;
  free(monster);
  free(this_event);
}

void pc_cleanup(dungeon_t* dungeon, event_t* this_event)
{
  pc_event_t* pc_event = ((pc_event_t*)this_event);
  pc_t* pc = pc_event->pc;
  point_t loc = ((character_t*)pc)->loc;
  dungeon->characters[loc.y][loc.x] = NULL;
  free(pc);
  free(this_event);
}

void add_monsters(dungeon_t* dungeon, int num_monsters)
{
  dungeon->num_characters += num_monsters;
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
	((event_t*)event)->time = 100/((character_t*)monster)->speed;
	((event_t*)event)->run = monster_take_turn;
	((event_t*)event)->cleanup = monster_cleanup;
	event->monster = monster;
	add_event((event_t*)event);
	break;
      }
    }
  }
}

void add_pc_event(pc_t* pc)
{
  pc_event_t* pc_event = malloc(sizeof(pc_event_t));
  ((event_t*)pc_event)->time = ((character_t*)pc)->speed;
  ((event_t*)pc_event)->run = pc_take_turn;
  ((event_t*)pc_event)->cleanup = pc_cleanup;
  pc_event->pc = pc;
  add_event((event_t*)pc_event);
}
