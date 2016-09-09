#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "dungeon/dungeons.h"
#include "save.h"

int main(int argc, char *argv[])
{

  check_make_dir();

  char round_rooms = 0;
  char pretty = 0;
  char load = 0;
  char save = 0;
  int args_processed = 0;
  char* load_name = "dungeon.rlg";
  char free_load_name = 0;
  char* save_name = "dungeon.rlg";
  char free_save_name = 0;
  while (argc > args_processed)
  {
    if (!strcmp("--round", argv[args_processed]))
    {
      round_rooms = 1;
    }
    else if (!strcmp("--pretty", argv[args_processed]))
    {
      pretty = 1;
    }
    else if (!strcmp("--load", argv[args_processed]))
    {
      load = 1;
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
	      !strcmp(name + strlen(name) - 4, ".rlg"))
	  {
	    load_name = name;
	  }
	  else
	  {
	    free_load_name = 1;
	    load_name = malloc(strlen(name) + 5);
	    strcpy(load_name, name);
	    strcpy(load_name + strlen(name), ".rlg");
	  }
	  args_processed++;
	}
      }
    }
    else if (!strcmp("--save", argv[args_processed]))
    {
      save = 1;
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
	      !strcmp(name + strlen(name) - 4, ".rlg"))
	  {
	    save_name = name;
	  }
	  else
	  {
	    free_save_name = 1;
	    save_name = malloc(strlen(name) + 5);
	    strcpy(save_name, name);
	    strcpy(save_name + strlen(name), ".rlg");
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
    dungeon = get_dungeon(21, 80, !round_rooms);
  }

  if (save && round_rooms && !load)
  {
    printf("INFO: Saving a dungeon generated with round rooms, distortion is likely (some floor areas may convert to halls)\n");
  }

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

  char wall = ' ';
  char hall = '#';
  char floor = '.';

  if (pretty)
  {
    wall = '$';
    hall = ' ';
    floor = ' ';
  }
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      if (dungeon->tiles[r][c].type == WALL)
      {
	if (dungeon->tiles[r][c].hardness < 100)
	{
	  printf("%c", wall);
	}
	else
	{
	  //printf("0");
	  printf("%c", wall);
	}
      }
      else if (dungeon->tiles[r][c].type == FLOOR)
      {
        printf("%c", floor);
      }
      else if (dungeon->tiles[r][c].type == HALL)
      {
        printf("%c", hall);
      }
    }
    printf("\n");
  }
  for (r = 0; r < 21; r++)
  {
    free(dungeon->tiles[r]);
  }
  free(dungeon->tiles);
  free(dungeon);
  return 0;
}
