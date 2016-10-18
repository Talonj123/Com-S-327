#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "dungeon/dungeons.h"
#include "save.h"
#include "pathfinding.h"
#include "gameflow.h"
#include <ncurses.h>


int main(int argc, char *argv[])
{

  check_make_dir();
  init_io();

  char load = 0;
  char save = 0;
  char num_monsters = 5;
  int args_processed = 1;
  char* load_name = "dungeon.rlg327";
  char free_load_name = 0;
  char* save_name = "dungeon.rlg327";
  char free_save_name = 0;
  while (argc > args_processed)
  {
    if (!strcmp("--nummon", argv[args_processed]))
    {
      if (argc > args_processed + 1)
      {
	int new_num = atoi(argv[args_processed + 1]);
	if (new_num >= 0)
	{
	  num_monsters = new_num;
	}
	args_processed++;
      }
    }
    else if (!strcmp("--load", argv[args_processed]))
    {
      load = 1;
      if (free_load_name)
      {
	free(load_name);
      }
      if (argc > args_processed + 1)
      {
	/* try parsing a name */
	char* name = argv[args_processed + 1];
        if (strncmp(name, "--", 2) &&
	    strncmp(name, ".", 1) &&
	    strncmp(name, "/", 1))
	{
	  /* not a switch, and basic checks passed, assume name */
	  if (strlen(name) > 4 &&
	      !strcmp(name + strlen(name) - 7, ".rlg327"))
	  {
	    load_name = name;
	  }
	  else
	  {
	    free_load_name = 1;
	    load_name = malloc(strlen(name) + 8);
	    strcpy(load_name, name);
	    strcpy(load_name + strlen(name), ".rlg327");
	  }
	  args_processed++;
	}
      }
    }
    else if (!strcmp("--save", argv[args_processed]))
    {
      save = 1;
      if (free_save_name)
      {
	free(save_name);
      }
      if (argc > args_processed + 1)
      {
	/* try parsing a name */
	char* name = argv[args_processed + 1];
        if (strncmp(name, "--", 2) &&
	    strncmp(name, ".", 1) &&
	    strncmp(name, "/", 1))
	{
	  /* not a switch, and basic checks passed, assume name */
	  if (strlen(name) > 4 &&
	      !strcmp(name + strlen(name) - 7, ".rlg327"))
	  {
	    save_name = name;
	  }
	  else
	  {
	    free_save_name = 1;
	    save_name = malloc(strlen(name) + 8);
	    strcpy(save_name, name);
	    strcpy(save_name + strlen(name), ".rlg327");
	  }
	  args_processed++;
	}
      }
    }
    args_processed++;
  }

  srand(time(0));
  dungeon_t* dungeon;

  if (load)
  {
    dungeon = load_dungeon(load_name);
    if (dungeon == NULL)
    {
      printf("dungeon not loaded: file does not exist (\"%s\")\n", load_name);
    }
    if (free_load_name)
    {
      free(load_name);
    }
    if (dungeon == NULL)
    {
      return 1;
    }
  }
  else
  {
    dungeon = dungeon_new();
  }
  
  add_monsters(dungeon, num_monsters);

  if (save)
  {
    if (strcmp(save_name, "dungeon.rlg"))
    {
      printf("Creating dungeon save in file: \"%s\"\n", save_name);
    }
    save_dungeon(dungeon, save_name);
    if (free_save_name)
    {
      free(save_name);
    }
  }
  get_distances(dungeon);

  print_dungeon(dungeon);
  
  pc_t* pc = dungeon->pc;
  add_pc_event(pc);
  while (((character_t*)pc)->alive && dungeon->num_characters > 1)
  {
    do_next_event(dungeon);
  }
  print_dungeon(dungeon);

  if (((character_t*)dungeon->pc)->alive)
  {
    mvprintw(22, 0, "--------------------------------------------------------------------------------");
    mvprintw(23, 0, "The PC caught them all!\n\n");
  }
  else
  {
    mvprintw(22, 0, "--------------------------------------------------------------------------------");
    mvprintw(23, 0, "The PC was pwned by a grue.\n\n");
  }

  clear_events(dungeon);
  dungeon_free(dungeon);

  getch();
  
  end_io();
  return 0;
}
