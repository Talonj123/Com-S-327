#include "characters.h"
#include "gameflow.h"
#include "pathfinding.h"
#include "io.h"

#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>

#define ERRATIC (0x08)
#define TUNNELING (0x04)
#define TELEPATHIC (0x02)
#define INTELLIGENT (0x01)

game_state_t game_state;

typedef struct monster_event
{
  event_t base;
  monster* target;

} monster_event_t;

typedef struct pc_event
{
  event_t base;
  player* pc;

} pc_event_t;

player* get_new_pc()
{
  player* pc = new player();
  pc->symbol = '@';
  pc->colors.push_back(COLOR_WHITE);
  pc->speed = 10;
  pc->type = PC;
  pc->alive = 1;
  pc->target = NULL;
  int r, c;
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      pc->memory.terrain[r][c] = WALL;
    } 
  }
  return pc;
}

monster::monster()
{
  alive = 1;
  last_known_pc.x = 0;
  last_known_pc.y = 0;
}

void move_character(dungeon_t* dungeon, character* character, point_t nloc, char tunneling)
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

  //character* other = NULL;
  if (dungeon->characters[nloc.y][nloc.x] != NULL)
  {
    //character* that;
    //    that = ;
    dungeon->characters[nloc.y][nloc.x]->alive = 0;
    /* let the event handler clean it up */
    dungeon->num_characters--;
  }
  dungeon->characters[nloc.y][nloc.x] = character;
  dungeon->characters[loc.y][loc.x] = NULL;
  character->loc = nloc;
}

char character_try_move_random(dungeon_t* dungeon, character* character, char tunneling)
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

char has_los_to_pc(dungeon_t* dungeon, monster* monster)
{
  int room_num;
  for (room_num = 0; room_num < dungeon->num_rooms; room_num++)
  {
    rectangle_t room = dungeon->rooms[room_num];
    if (rect_contains_point(room, dungeon->pc->loc)
	&& rect_contains_point(room, monster->loc))
    {
      return 1;
    }
  }
  return 0;
}


void move_toward_pc_path(dungeon_t* dungeon, monster* monster)
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
  point_t loc = monster->loc;
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
  move_character(dungeon, monster, nloc, monster->attributes.tunneling);
}

void move_toward_pc_line(dungeon_t* dungeon, monster* monster)
{
  point_t pcloc = monster->last_known_pc;
  point_t monloc = monster->loc;
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
  move_character(dungeon, monster, nloc, monster->attributes.tunneling);
}

void monster_take_turn(dungeon_t* dungeon, event_t* this_event)
{
  monster* monster = ((monster_event_t*)this_event)->target;
  if (!monster->alive)
  {
    /* delete monster and  don't re-add event */
    /* removed from character map already when killed */
    delete monster;
    free(this_event);
    return;
  }

  if (monster->attributes.telepathic)
  {
    monster->last_known_pc = dungeon->pc->loc;
  }
  else if (has_los_to_pc(dungeon, monster))
  {
    monster->last_known_pc = dungeon->pc->loc;
  }
  else if (!monster->attributes.intelligent)
  {
    monster->last_known_pc.x = 0;
    monster->last_known_pc.y = 0;
  }

  if (monster->attributes.erratic && (rand() % 2))
  {
    int tries = 0;
    while (tries++ < 5)
    {
      if (character_try_move_random(dungeon, monster, monster->attributes.tunneling))
	break;
    }
  } else
  {
    /* other cases */
    if (monster->attributes.telepathic && monster->attributes.intelligent)
    {
      move_toward_pc_path(dungeon, monster);
    }
    else if (monster->last_known_pc.x > 0 &&
	     monster->last_known_pc.x < DUNGEON_COLS - 1 &&
	     monster->last_known_pc.y > 0 &&
	     monster->last_known_pc.y < DUNGEON_ROWS - 1)
    {
      move_toward_pc_line(dungeon, monster);
    }
    else
    {
      character_try_move_random(dungeon, monster, monster->attributes.tunneling);
    }
  }

  this_event->time += 100/monster->speed;
  add_event((event_t*)this_event);
}

char pc_try_move(dungeon_t* dungeon, player* pc, int dx, int dy)
{
  char tunneling = 0;
  point_t loc = pc->loc;
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
    character* character = dungeon->characters[nloc.y][nloc.x];
    character->alive = 0;
    /* let the event handler clean it up */
    dungeon->num_characters--;
  }
  dungeon->characters[nloc.y][nloc.x] = pc;
  dungeon->characters[loc.y][loc.x] = NULL;
  pc->loc = nloc;
  return 1;
}

void pc_take_turn(dungeon_t* dungeon, event_t* this_event)
{
  player* pc = ((pc_event_t*)this_event)->pc;
  if (!pc->alive)
  {
    game_state.running = 0;
    free(this_event);
    return;
  }

  pc->UpdateMemory(dungeon);

  pc_turn_interface(dungeon, pc);

  this_event->time += 100/pc->speed;
  add_event(this_event);
}

void monster_cleanup(dungeon_t* dungeon, event_t* this_event)
{
  monster_event_t* monster_event = ((monster_event_t*)this_event);
  monster* monster = monster_event->target;
  point_t loc = monster->loc;
  dungeon->characters[loc.y][loc.x] = NULL;
  delete monster;
  free(this_event);
}

void pc_cleanup(dungeon_t* dungeon, event_t* this_event)
{
  // Don't free the PC, it may be used on the next level
  free(this_event);
}

void free_pc(player* pc)
{
  delete pc;
}

void free_character(character* character)
{
  delete character;
}

void add_monsters(dungeon_t* dungeon, std::vector<monster_data> types, int num_monsters)
{
  dungeon->num_characters += num_monsters;
  int i;
  for (i = 0; i < num_monsters; i++)
  {
    monster* monster = types[rand()%types.size()].create();
    while (1)
    {
      int r = rand() % DUNGEON_ROWS;
      int c = rand() % DUNGEON_COLS;
      if (dungeon->terrain[r][c] != WALL &&
	  dungeon->characters[r][c] == NULL)
      {
	monster->loc.x = c;
	monster->loc.y = r;
	dungeon->characters[r][c] = monster;
	monster_event_t* event = (monster_event_t*)malloc(sizeof(monster_event_t));
	((event_t*)event)->time = 100/monster->speed;
	((event_t*)event)->run = monster_take_turn;
	((event_t*)event)->cleanup = monster_cleanup;
	event->target = monster;
	add_event((event_t*)event);
	break;
      }
    }
  }
}

void add_pc_event(player* pc)
{
  pc_event_t* pc_event = (pc_event_t*)malloc(sizeof(pc_event_t));
  ((event_t*)pc_event)->time = 0;
  ((event_t*)pc_event)->run = pc_take_turn;
  ((event_t*)pc_event)->cleanup = pc_cleanup;
  pc_event->pc = pc;
  add_event((event_t*)pc_event);
}

void set_character_loc(character* character, point_t point)
{
  character->loc = point;
}

char get_character_symbol(character* character)
{
  return character->symbol;
}

point_t get_character_loc(character* character)
{
  return character->loc;
}

CharacterType get_character_type(character* character)
{
  return character->type;
}

PlayerMemory get_pc_memory(player* pc)
{
  return pc->memory;
}

void player::UpdateMemory(dungeon_t* dungeon)
{
  int dy, dx;
  for (dy = -3; dy <= 3; dy++)
  {
    for (dx = -3; dx <= 3; dx++)
    {
      if (loc.y + dy >= 0 && loc.y + dy < DUNGEON_ROWS &&
	  loc.x + dx >= 0 && loc.x + dx < DUNGEON_COLS)
      {
	memory.terrain[loc.y+dy][loc.x+dx] = dungeon->terrain[loc.y+dy][loc.x+dx];
      }
    }
  }
}

void clear_pc_memory(player* pc)
{
  int r, c;
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      pc->memory.terrain[r][c] = WALL;
    }
  }
}
