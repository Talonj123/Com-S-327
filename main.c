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

void print_dungeon(dungeon_t* dungeon, char wall, char hall, char floor);
void print_types(dungeon_t* dungeon, char wall, char hall, char floor);
void print_distances_non_tunneling(dungeon_t* dungeon, char wall, char hall, char floor);
void print_distances_tunneling(dungeon_t* dungeon, char wall, char hall, char floor);

int main(int argc, char *argv[])
{

  check_make_dir();

  char pretty = 0;
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
    if (!strcmp("--pretty", argv[args_processed]))
    {
      pretty = 1;
    }
    else if (!strcmp("--nummon", argv[args_processed]))
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
  char wall = ' ';
  char hall = '#';
  char floor = '.';

  if (pretty)
  {
    wall = '$';
    hall = ' ';
    floor = ' ';
  }
  print_dungeon(dungeon, wall, hall, floor);
  print_distances_non_tunneling(dungeon, wall, hall, floor);
  print_distances_tunneling(dungeon, wall, hall, floor);

  while (((character_t*)dungeon->pc)->alive)
  {
    usleep(100000);
    do_next_event(dungeon);
    print_dungeon(dungeon, wall, hall, floor);
  }

  dungeon_free(dungeon);
  return 0;
}

void print_dungeon(dungeon_t* dungeon, char wall, char hall, char floor)
{
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_type tile = dungeon->terrain[r][c];
      if (dungeon->characters[r][c] != NULL)
      {
	printf("%c", dungeon->characters[r][c]->symbol);
      }	
      else if (tile == WALL)
      {
	printf("%c", wall);
      }
      else if (tile == FLOOR)
      {
        printf("%c", floor);
      }
      else if (tile == HALL)
      {
        printf("%c", hall);
      }
    }
    printf("\n");
  }
}

void print_types(dungeon_t* dungeon, char wall, char hall, char floor)
{
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      if (dungeon->terrain[r][c] == WALL)
      {
	if (dungeon->hardness[r][c] < 100)
	{
	  printf("%c", wall);
	}
	else
	{
	  //printf("0");
	  printf("%c", wall);
	}
      }
      else if (dungeon->terrain[r][c] == FLOOR)
      {
        printf("%c", floor);
      }
      else if (dungeon->terrain[r][c] == HALL)
      {
        printf("%c", hall);
      }
    }
    printf("\n");
  }
}

void print_distances_non_tunneling(dungeon_t* dungeon, char wall, char hall, char floor)
{
  char distance_markers[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_type tile = dungeon->terrain[r][c];
      if (dungeon->distance_to_pc[r][c] < sizeof(distance_markers)-1)
      {
	printf("%c", distance_markers[dungeon->distance_to_pc[r][c]]);
      }
      else
      {
        if (tile == WALL)
	{
	  printf("%c", wall);
	}
	else if (tile == FLOOR)
	{
	  printf("%c", floor);
	}
	else if (tile == HALL)
	{
	  printf("%c", hall);
	}
      }
    }
    printf("\n");
  }
}


void print_distances_tunneling(dungeon_t* dungeon, char wall, char hall, char floor)
{
  char distance_markers[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_type tile = dungeon->terrain[r][c];
      if (dungeon->tunneling_distance_to_pc[r][c] < sizeof(distance_markers)-1)
      {
	printf("%c", distance_markers[dungeon->tunneling_distance_to_pc[r][c]]);
      }
      else
      {
        if (tile == WALL)
	{
	  printf("%c", wall);
	}
	else if (tile == FLOOR)
	{
	  printf("%c", floor);
	}
	else if (tile == HALL)
	{
	  printf("%c", hall);
	}
      }
    }
    printf("\n");
  }
}
