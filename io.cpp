#include "characters.h"
#include "gameflow.h"
#include "pathfinding.h"
#include "io.h"
#include "items.hpp"

#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>

#define SPACES8 "        "
#define SPACES10 "          "
#define SPACES30 "                              "
#define SPACES80 "                                                                                "

#define DASHES8 "--------"
#define DASHES10 "----------"
#define DASHES30 "------------------------------"

using namespace std;

void init_color_pairs();
void wear_item_interface(player* pc);
void take_off_item_interface(player* pc);
void drop_item_interface(dungeon* dungeon, player* pc);
void expunge_item_interface(player* pc);
void inventory_interface(dungeon* dungeon, player* pc);
void inspect_inventory_interface(player* pc);
void equipment_interface(dungeon* dungeon, player* pc);
void inventory_interface(dungeon* dungeon, player* pc);
void inspect_inventory_interface(player* pc);
void monster_list_interface(dungeon_t* dungeon);
void print_inventory(const player *pc, int selected = -1);
void print_equipment(const player *pc, int selected = -1);

void monster_list_interface(dungeon_t* dungeon)
{
  int num_monsters = dungeon->num_characters-1;
  character** character_list = (character**)calloc(num_monsters, sizeof(character*));
  int r, c, i = 0;
  fprintf(stderr, "%d\r\n", num_monsters);
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      character* character = dungeon->characters[r][c];
      if (character != NULL && character->type == MONSTER)
      {
	//**DEBUG
	int k;
	for (k = 0; k < i; k++)
	{
	  if (character_list[k] == character)
	  {
	    fprintf(stderr, "DUPLICATE: list[%d]==list[%d]\r\n", k, i);
	  }
	}
	fprintf(stderr, "%d\r\n", i);
	character_list[i++] = character;
	get_character_loc(character);
      }
    }
  }
  point_t loc = get_character_loc((character*)dungeon->get_pc());
  char **lines = (char**)malloc(sizeof(char*)*num_monsters);
  for (i = 0; i < num_monsters; i++)
  {
    lines[i] = (char*)malloc(sizeof(char)*35);
    character* monster = character_list[i];
    point_t mloc = monster->loc;
    point_t diff = { mloc.x - loc.x, mloc.y - loc.y };
    sprintf(lines[i], "| A %c, %2d %5s and %2d %4s |",
	    get_character_symbol(monster),
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

void wear_item_interface(player* pc)
{
  int i;
  for (i = 0; i < 10; i++)
  {
    if (pc->carry[i] != NULL)
    {
      break;
    }
  }
  if (i >= 10)
  {
    mvprintw(22, 0, "Inventory is empty :/                                    ");
    return;
  }
  mvprintw(22, 0, "Select a slot to wear: (0-9)                                   ");
  while (1)
  {
    print_inventory(pc);
    int key = getch();
    if (key == 033)
    {
      break;
    }
    else if (key >= 060 && key <= 071)
    {
      int index = key - 060;
      ::item *to_wear = pc->carry[index];
      if (to_wear == NULL)
      {
	mvprintw(22, 0, "There's nothing in slot %d.                         ", index);
      }
      else if (to_wear->get_type() < WEAPON || to_wear->get_type() > RING)
      {
	mvprintw(22, 0, "The item in  slot %d isn't wearable.                         ", index);
      }
      else if (to_wear->get_type() == RING)
      {
	char hand = ' ';
	do
	{
	  mvprintw(22, 0, "Enter a hand to put the ring on (r/l): %c                         ", hand);
	  refresh();
	  hand = getch();
	} while (hand != 'r' && hand != 'l');
	pc->equip(index, (hand == 'r') ? 0 : 1);
      }
      else
      {
	pc->equip(index);
      }
    }
  }
}

void take_off_item_interface(player* pc)
{
  int i;
  for (i = 0; i < 10; i++)
  {
    if (pc->carry[i] == NULL)
    {
      break;
    }
  }
  if (i > 10)
  {
    mvprintw(22, 0, "Inventory is full :/                                    ");
    return;
  }
  mvprintw(22, 0, "Select a slot to take off: (a-l)                                   ");
  while (1)
  {
    print_equipment(pc);
    int key = getch();
    if (key == 033)
    {
      break;
    }
    else if (key >= 'a' && key <= 'l')
    {
      int index = key - 'a';
      ::item *to_doff = pc->equipment[index];
      if (to_doff == NULL)
      {
	mvprintw(22, 0, "There's nothing in slot %c.                         ", key);
      }
      else
      {
	pc->carry[i] = pc->unequip(index);
      }
    }
  }
}

void drop_item_interface(dungeon* dungeon, player* pc)
{
  int i;
  for (i = 0; i < 10; i++)
  {
    if (pc->carry[i] != NULL)
    {
      break;
    }
  }
  if (i >= 10)
  {
    mvprintw(22, 0, "Inventory is empty :/                                    ");
    return;
  }
  mvprintw(22, 0, "Select a slot to drop: (0-9)                                   ");
  while (1)
  {
    print_inventory(pc);
    int key = getch();
    if (key == 033)
    {
      break;
    }
    else if (key >= 060 && key <= 071)
    {
      int index = key - 060;
      ::item *to_drop = pc->carry[index];
      if (to_drop == NULL)
      {
	mvprintw(22, 0, "There's nothing in slot %d.                         ", index);
      }
      else
      {
	pc->carry[index] = dungeon->items[pc->loc.y][pc->loc.x];
	dungeon->items[pc->loc.y][pc->loc.x] = to_drop;
      }
    }
  }
}

void expunge_item_interface(player* pc)
{

  int i;
  for (i = 0; i < 10; i++)
  {
    if (pc->carry[i] != NULL)
    {
      break;
    }
  }
  if (i >= 10)
  {
    mvprintw(22, 0, "Inventory is empty :/                                    ");
    return;
  }
  mvprintw(22, 0, "Select a slot to destroy: (0-9)                                   ");
  while (1)
  {
    print_inventory(pc);
    int key = getch();
    if (key == 033)
    {
      break;
    }
    else if (key >= 060 && key <= 071)
    {
      int index = key - 060;
      ::item *to_drop = pc->carry[index];
      if (to_drop == NULL)
      {
	mvprintw(22, 0, "There's nothing in slot %d.                         ", index);
      }
      else
      {
	pc->carry[index] = NULL;
	delete to_drop;
      }
    }
  }
}

void inventory_interface(dungeon* dungeon, player* pc)
{
  int selected = 0;
  while (true)
  {
    print_inventory(pc, selected);
    int row = 16, i;
    for (i = 0; i < 24-row; i++)
    {
      if (pc->carry[selected] != NULL)
      {
	string desc = pc->carry[selected]->get_description();
	int length = desc.length();
	if (length > i*80)
	{
	  string line = desc.substr(i*80, (i+1)*80 + 1).append(80 - (length % 80), ' ');
	  mvprintw(row + i, 0, "%s", line.c_str());
	  continue;
	}
      }
	mvprintw(row + i, 0, "                                                                                ");
    }
    int key = getch();
    if (key == 033)
    {
      break;
    }
    else if ((key == 0403) && (selected > 0))
    {
      selected--;
    }
    else if ((key == 0402) && (selected < 9))
    {
      selected++;
    }
    else if (key == 'x')
    {
      delete pc->carry[selected];
      pc->carry[selected] = NULL;
    }
    else if (key == 'd')
    {
      if (pc->carry[selected] != NULL)
      {
	item* holder = pc->carry[selected];
	pc->carry[selected] = dungeon->items[pc->loc.y][pc->loc.x];
	dungeon->items[pc->loc.y][pc->loc.x] = holder;
      }
    }
    else if (key == 'w')
    {
      if (pc->carry[selected] != NULL)
      {
	if (pc->carry[selected]->get_type() < WEAPON || pc->carry[selected]->get_type() > RING)
	{
	  mvprintw(22, 0, "The item in  slot %d isn't wearable.                         ", selected);
	}
	else if (pc->carry[selected]->get_type() == RING)
	{
	  char hand = ' ';
	  do
	  {
	    mvprintw(22, 0, "Enter a hand to put the ring on (r/l): %c                         ", hand);
	    refresh();
	    hand = getch();
	  } while (hand != 'r' && hand != 'l');
	  pc->equip(selected, (hand == 'r') ? 0 : 1);
	}
	else
	{
	  pc->equip(selected);
	}
      }
    }
  }
}

void equipment_interface(dungeon* dungeon, player* pc)
{
  int selected = 0;
  while (true)
  {
    print_equipment(pc, selected);
    int row = 17, i;
    for (i = 0; i < 26-row; i++)
    {
      if (pc->equipment[selected] != NULL)
      {
	string desc = pc->equipment[selected]->get_description();
	int length = desc.length();
	if (length > i*80)
	{
	  string line = desc.substr(i*80, (i+1)*80 + 1).append(80 - (length % 80), ' ');
	  mvprintw(row + i, 0, "%s", line.c_str());
	  continue;
	}
      }
      mvprintw(row + i, 0, "                                                                                ");
    }
    int key = getch();
    if (key == 033)
    {
      break;
    }
    else if ((key == 0403) && (selected > 0))
    {
      selected--;
    }
    else if ((key == 0402) && (selected < 11))
    {
      selected++;
    }
    else if (key == 'x')
    {
      if (pc->equipment[selected] != NULL)
      {
	delete pc->unequip(selected);
      }
    }
    else if (key == 'd')
    {
      if (pc->equipment[selected] != NULL)
      {
	dungeon->items[pc->loc.y][pc->loc.x] = pc->unequip(selected);
      }
    }
    else if (key == 't')
    {
      if (pc->equipment[selected] != NULL)
      {
	int i;
	for (i = 0; i < 10; i++)
	{
	  if (pc->carry[i] != NULL)
	  {
	    break;
	  }
	}
	if (i >= 10)
	{
	  mvprintw(22, 0, "Inventory is full :/                                    ");
	  continue;
	}
	pc->carry[i] = pc->unequip(selected);
      }
    }
  }
}

void inspect_inventory_interface(player* pc)
{
  int selected = 0;
  while (true)
  {
    print_inventory(pc, selected);
    int row = 16, i;
    for (i = 0; i < 24-row; i++)
    {
      if (pc->carry[selected] != NULL)
      {
	string desc = pc->carry[selected]->get_description();
	int length = desc.length();
	if (length > i*80)
	{
	  string line = desc.substr(i*80, (i+1)*80 + 1).append(length % 80, ' ');
	  mvprintw(row + i, 0, "%s", line.c_str());
	  continue;
	}
      }
	mvprintw(row + i, 0, "                                                                                ");
    }
    int key = getch();
    if (key == 033)
    {
      break;
    }
    else if ((key == 0403) && (selected > 0))
    {
      selected--;
    }
    else if ((key == 0402) && (selected < 9))
    {
      selected++;
    }
  }
}

void pc_turn_interface(dungeon_t* dungeon, player* pc)
{
  if (!pc->alive)
  {
    fprintf(stderr, "PC DEAD, BUT INTERFACE CALLED\n");
  }
  print_dungeon(dungeon);
  mvprintw(0, 20, "HP: %d/%d", pc->hitpoints, pc->hitpoints_max);
  point_t loc = get_character_loc((character*)pc);

  int ch;
GETCHAR_LBL:
  pc->UpdateMemory(dungeon);
  ch = getch();
  mvprintw(0, 0, SPACES80);
  mvprintw(0, 0, "Key: %#o", ch);
  refresh();
  switch (ch)
  {
  default:
    goto GETCHAR_LBL;

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
  case 'w':
    wear_item_interface(pc);
    pc->UpdateMemory(dungeon);
    print_dungeon(dungeon);
    goto GETCHAR_LBL;
  case 't':
    take_off_item_interface(pc);
    print_dungeon(dungeon);
    goto GETCHAR_LBL;
  case 'd':
    drop_item_interface(dungeon, pc);
    print_dungeon(dungeon);
    goto GETCHAR_LBL;
  case 'x':
    expunge_item_interface(pc);
    print_dungeon(dungeon);
    goto GETCHAR_LBL;
  case 'i':
    inventory_interface(dungeon, pc);
    pc->UpdateMemory(dungeon);
    print_dungeon(dungeon);
    goto GETCHAR_LBL;
  case 'e':
    equipment_interface(dungeon, pc);
    print_dungeon(dungeon);
    goto GETCHAR_LBL;
  case 'I':
    inspect_inventory_interface(pc);
    print_dungeon(dungeon);
    goto GETCHAR_LBL;
  };
  mvprintw(22, 0, SPACES80);
  mvprintw(23, 0, SPACES80);
  item* item = dungeon->items[pc->loc.y][pc->loc.x];
  //item* item = NULL;
  if (item != NULL)
  {
    string name = item->get_name();
    bool has_empty = false;
    unsigned int i;
    for (i = 0; i < 10; i++)
    {
      if (pc->carry[i] == NULL)
      {
	has_empty = true;
	break;
      }
    }
    if (has_empty)
    {
      // pick up item
      pc->carry[i] = item;
      dungeon->items[pc->loc.y][pc->loc.x] = NULL;
      mvprintw(22, 0, "Picked up ");
      if (name.length() > 40)
      {
	name = name.substr(0, 37);
	mvprintw(22, 10, (name + "...").c_str());
      }
      else
      {
	mvprintw(22, 10, name.c_str());
      }
    }
    else
    {
      if (name.length() > 40)
      {
	name = name.substr(0, 37);
	mvprintw(22, 0, (name + "...").c_str());
      }
      else
      {
	mvprintw(22, 0, name.c_str());
      }
    }

    if (item->get_description().length() > 80)
    {
      mvprintw(23, 0, (item->get_description().substr(0, 77) + "...").c_str());
    }
    else
    {
      mvprintw(23, 0, item->get_description().c_str());
    }
    refresh();
  }
}

void print_dungeon(dungeon_t* dungeon)
{
  const PlayerMemory memory = get_pc_memory(dungeon->get_pc());
  point_t loc = get_character_loc((character*)dungeon->get_pc());

  int radius = 3;
  if (dungeon->get_pc()->equipment[LIGHT] != NULL)
  {
    radius += dungeon->get_pc()->equipment[LIGHT]->get_special();
  }

  int r, c;
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      tile_type tile = memory.terrain[r][c];
      item* item = memory.items[r][c];
      //tile_type tile = dungeon->terrain[r][c];
      char ch = ' ';
      int color = COLOR_WHITE;
      if (dungeon->characters[r][c] != NULL && (abs(loc.x - c) <= radius) && (abs(loc.y - r) <= radius))
      {
	character* character = dungeon->characters[r][c];
        ch = get_character_symbol(character);
	color = character->colors[0];
      }
      else if (item != NULL)
      {
	ch = item->get_symbol();
	color = item->get_colors()[0];
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
      if (abs(loc.x - c) <= radius && abs(loc.y - r) <= radius)
      {
	attron(A_BOLD);
      }
      attron(COLOR_PAIR(color));
      mvaddch(r+1, c, ch);
      attroff(COLOR_PAIR(color));
      if (abs(loc.x - c) <= radius && abs(loc.y - r) <= radius)
      {
	attroff(A_BOLD);
      }
    }
  }
  refresh();
}

void print_inventory(const player *pc, int selected)
{
  char *lines[10];
  int i;
  for (i = 0; i < 10; i++)
  {
    lines[i] = (char*)malloc(sizeof(char)*41);
    item* item = pc->carry[i];
    if (item != NULL)
    {
      sprintf(lines[i], "| %1d: %7s %25s |",
	      i,
	      object_data::get_type_name(item->get_type()).c_str(),
	      item->get_name().substr(0, 25).c_str());
    }
    else
    {
      sprintf(lines[i], "| %1d:     " SPACES30 "|",
	      i);
    }
  }
  mvprintw((21-10)/2 - 1, (80-40)/2, DASHES10 DASHES30);
  mvprintw((21-10)/2 + 10, (80-40)/2, DASHES10 DASHES30);
  for (i = 0; i < 10; i++)
  {
    char* str = lines[i];
    if (i == selected)
    {
      attron(COLOR_PAIR(COLOR_GREEN));
    }
    mvprintw(i+(21-10)/2, (80-strlen(str))/2, str);
    if (i == selected)
    {
      attroff(COLOR_PAIR(COLOR_GREEN));
    }
  }
  refresh();  
  for (i = 0; i < 10; i++)
  {
    free(lines[i]);
  }  
}

void print_equipment(const player *pc, int selected)
{
  char *lines[12];
  int i;
  for (i = 0; i < 12; i++)
  {
    lines[i] = (char*)malloc(sizeof(char)*41);
    item* item = pc->equipment[i];
    if (item != NULL)
    {
      sprintf(lines[i], "| %c: %7s %25s |",
	      i + 'a',
	      object_data::get_type_name(item->get_type()).c_str(),
	      item->get_name().substr(0, 25).c_str());
    }
    else
    {
      sprintf(lines[i], "| %c: %7s" SPACES10 SPACES10 "       |",
	      i + 'a', object_data::get_type_name((item_type)(i - i/11)).c_str());
    }
  }
  mvprintw((21-12)/2 - 1, (80-40)/2, DASHES10 DASHES30);
  mvprintw((21-12)/2 + 12, (80-40)/2, DASHES10 DASHES30);
  for (i = 0; i < 12; i++)
  {
    char* str = lines[i];
    if (i == selected)
    {
      attron(COLOR_PAIR(COLOR_GREEN));
    }
    mvprintw(i+(21-12)/2, (80-strlen(str))/2, str);
    if (i == selected)
    {
      attroff(COLOR_PAIR(COLOR_GREEN));
    }
  }
  refresh();  
  for (i = 0; i < 12; i++)
  {
    free(lines[i]);
  }  
}

void init_io()
{
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  set_escdelay(100);
  start_color();
  init_color_pairs();
}

void init_color_pairs()
{
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void end_io()
{
  endwin();
}
