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

void monster_list_interface(dungeon_t* dungeon)
{
  int num_monsters = dungeon->num_characters-1;
  character_t** character_list = malloc(num_monsters*sizeof(character_t*));
  int r, c, i = 0;
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      character_t* character = dungeon->characters[r][c];
      if (character != NULL && character->type == MONSTER)
      {
	character_list[i++] = character;
      }
    }
  }
  point_t loc = ((character_t*)dungeon->pc)->loc;
  char **lines = malloc(sizeof(char*)*num_monsters);
  for (i = 0; i < dungeon->num_characters-1; i++)
  {
    lines[i] = malloc(sizeof(char)*35);
    character_t* monster = character_list[i];
    point_t mloc = monster->loc;
    point_t diff = { mloc.x - loc.x, mloc.y - loc.y };
    sprintf(lines[i], "| A %c, %2d %5s and %2d %4s |",
	    monster->symbol,
	    abs(diff.y),
	    (diff.y >= 0) ? "south" : "north",
	    abs(diff.x),
	    (diff.x >= 0) ? "east" : "west");
  }
  free(character_list);
  int offset = 0;
  const int max_display = 10;
  while (1)
  {
    //mvprintw((21-max_display)/2 - 1, (80-MAX_STRLEN)/2, "                             ");
    //mvprintw((21-max_display)/2 + max_display, (80-MAX_STRLEN)/2, "                             ");

    if (offset == 0)
    {
      mvprintw((21-max_display)/2 - 1, (80-29)/2, "-----------------------------");
    }
    else
    {
      mvprintw((21-max_display)/2 - 1, (80-29)/2, "--------------^--------------");
    }

    if (offset + max_display < num_monsters)
    {
      mvprintw((21-max_display)/2 + max_display, (80-29)/2, "--------------,--------------");
    }
    else
    {
      mvprintw((21-max_display)/2 + max_display, (80-29)/2, "-----------------------------");
    }

    int upper = max_display;
    if (num_monsters - offset < max_display)
    {
      upper = num_monsters - offset;
    }
    for (i = 0; i < upper; i++)
    {
      char* str = lines[i+offset];
      mvprintw(i+(21-max_display)/2, (80-strlen(str))/2, str);
    }
    for (i = upper; i < max_display; i++)
    {
      mvprintw(i+(21-max_display)/2, (80-29)/2, "|                           |");
    }
    refresh();
    int key = getch();
    if (key == 0402)
    {
      offset += 1;
      if (offset + max_display > num_monsters)
      {
	offset = num_monsters - max_display;
	if (offset < 0)
	{
	  offset = 0;
	}
      }
    }
    else if (key == 0403)
    {
      offset -= 1;
      if (offset < 0)
      {
	offset = 0;
      }
    }
    else if (key == 033)
    {
      break;
    }
  }
  for (i = 0; i < num_monsters; i++)
  {
    free(lines[i]);
  }
  free(lines);
  
  print_dungeon(dungeon);
}

void pc_turn_interface(dungeon_t* dungeon, pc_t* pc)
{
 print_dungeon(dungeon);
  
  point_t loc = ((character_t*)pc)->loc;

  int ch;

GETCHAR_LBL:
  ch = getch();
  switch (ch)
  {
  default:
    mvprintw(0, 0, "Unknown Key: %#o  ", ch);
    goto GETCHAR_LBL;
    break;

    /* numpad.7 (up+left) */
  case 067:
  case 'y':
    /* try move up+left */
    pc_try_move(dungeon, pc, -1, -1);
    break;

    /* numpad.8 (up) */
  case 070:
  case 'k':
    /* try move up */
    pc_try_move(dungeon, pc, 0, -1);
    break;

    /* numpad.9 (up+right) */
  case 071:
  case 'u':
    /* try move up+right */
    pc_try_move(dungeon, pc, 1, -1);
    break;

    /* numpad.1 (down+left) */
  case 061:
  case 'b':
    /* try move down+left */
    pc_try_move(dungeon, pc, -1, 1);
    break;

    /* numpad.2 (down) */
  case 062:
  case 'j':
    /* try move down */
    pc_try_move(dungeon, pc, 0, 1);
    break;

    /* numpad.3 (down+right) */
  case 063:
  case 'n':
    /* try move down+left */
    pc_try_move(dungeon, pc, 1, 1);
    break;

    /* numpad.4 (left) */
  case 064:
  case 'h':
    /* try move left */
    pc_try_move(dungeon, pc, -1, 0);
    break;

    /* numpad.g (right) */
  case 066:
  case 'l':
    /* try move left */
    pc_try_move(dungeon, pc, 1, 0);
    break;

  case 'Q':
    game_state.quitted = 1;
    game_state.running = 0;
    break;

  case 065:
  case ' ':
    /* numpad.5 */
    break;

  case 'm':
    monster_list_interface(dungeon);
    goto GETCHAR_LBL;
    break;

  case '<':
    if (dungeon->terrain[loc.y][loc.x] == UP_STAIR)
    {
      game_state.reload = 1;
    }
    else
    {
      goto GETCHAR_LBL;
    }
    break;

  case '>':
    if (dungeon->terrain[loc.y][loc.x] == DOWN_STAIR)
    {
      game_state.reload = 1;
    }
    else
    {
      goto GETCHAR_LBL;
    }
    break;
  }
}

void print_dungeon(dungeon_t* dungeon)
{
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_type tile = dungeon->terrain[r][c];
      char ch = ' ';
      if (dungeon->characters[r][c] != NULL)
      {
        ch = dungeon->characters[r][c]->symbol;
      }
      else if (tile == WALL)
      {
	ch = ' ';
      }
      else if (tile == FLOOR)
      {
        ch = '.';
      }
      else if (tile == HALL)
      {
        ch = '#';
      }
      else if (tile == UP_STAIR)
      {
        ch = '<';
      }
      else if (tile == DOWN_STAIR)
      {
        ch = '>';
      }
      mvaddch(r, c, ch);
    }
  }
  refresh();
}

void init_io()
{
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  set_escdelay(100);
}

void end_io()
{
  endwin();
}
