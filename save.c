#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>

#include "dungeon/dungeons.h"

#include "dungeon/coordinates.h"

#define HARDNESS dungeon->hardness[r][c]
#define TERRAIN dungeon->terrain[r][c]

char* homedir = NULL;

char check_make_dir()
{
  char dir[] = "/.rlg327/";
  if (homedir == NULL)
  {
    homedir = getenv("HOME");
  }
  int total_length = strlen(homedir) + strlen(dir);

  char* expanded = malloc(total_length + 1);

  strcpy(expanded, homedir);
  strcpy(expanded + strlen(homedir), dir);
  expanded[total_length] = 0;

  char to_return = 0;
  struct stat st;
  if (stat(expanded, &st) != 0)
  {
    to_return = mkdir(expanded, 0x600) != 0;
  }
  free(expanded);
  return to_return;
}

char save_dungeon(const dungeon_t* dungeon, char* name)
{
  char error = 0;
  if (homedir == NULL)
  {
    homedir = getenv("HOME");
  }
  check_make_dir();
  char *save_dir = "/.rlg327/";
  int total_length = strlen(homedir) + strlen(save_dir) + strlen(name);
  char* expanded = malloc(total_length + 1);

  strcpy(expanded, homedir);
  strcpy(expanded + strlen(homedir), save_dir);
  strcpy(expanded + strlen(homedir) + strlen(save_dir), name);

  uint32_t size = 14 + (DUNGEON_ROWS * DUNGEON_COLS) + (dungeon->num_rooms*4);
  FILE* file = fopen(expanded, "w");
  if (file == NULL)
  {
    error = errno;
  }
  else
  {
    /* save the file */
    fprintf(file, "RLG327");
    uint32_t version = 0;
    
    version = htobe32(version);
    fwrite(&version, sizeof(version), 1, file);

    size = htobe32(size);
    fwrite(&size, sizeof(size), 1, file);

    fwrite(dungeon->hardness, sizeof(dungeon->hardness[0][0]), DUNGEON_ROWS * DUNGEON_COLS, file);

    int i;
    for (i = 0; i < dungeon->num_rooms; i++)
    {
      rectangle_t room = dungeon->rooms[i];
      char points[4];
      points[0] = room.x;
      points[1] = room.width;
      points[2] = room.y;
      points[3] = room.height;

      fwrite(points, 1, 4, file);
    }
    fclose(file);
  }

  free(expanded);
  return error;
}

dungeon_t* load_dungeon(char* name)
{
  if (homedir == NULL)
  {
    homedir = getenv("HOME");
  }
  check_make_dir();
  dungeon_t* dungeon = NULL;
  char *save_dir = "/.rlg327/";
  int total_length = strlen(homedir) + strlen(save_dir) + strlen(name);
  char* expanded = malloc(total_length + 1);

  strcpy(expanded, homedir);
  strcpy(expanded + strlen(homedir), save_dir);
  strcpy(expanded + strlen(homedir) + strlen(save_dir), name);
  expanded[total_length] = 0;

  FILE* file = fopen(expanded, "r");
  if (file != NULL)
  {
    char descriptor[7];
    descriptor[6] = 0;
    uint32_t version, size;

    fread(descriptor, 1, 6, file);
    fread(&version, 4, 1, file);
    fread(&size, 4, 1, file);
     
    version = be32toh(version);
    size = be32toh(size);
    
    dungeon = malloc(sizeof(dungeon_t));
    fread(dungeon->hardness, 1, DUNGEON_ROWS * DUNGEON_COLS, file);
    int r, c;
    for (r = 0; r < DUNGEON_ROWS; r++)
    {
      for (c = 0; c < DUNGEON_COLS; c++)
      {
	if (HARDNESS == 0)
	  TERRAIN = HALL;
	else
	  TERRAIN = WALL;
      }  
    }

    dungeon->num_rooms = (size - 14 - 80*21)/4;
    dungeon->rooms = malloc(sizeof(rectangle_t)*dungeon->num_rooms);
    int i;
    for (i = 0; i < dungeon->num_rooms; i++)
    {
      rectangle_t bounds;
      bounds.x = fgetc(file);
      bounds.width = fgetc(file);
      bounds.y = fgetc(file);
      bounds.height = fgetc(file);

      dungeon->rooms[i] = bounds;

      int r, c;
      for (r = bounds.y; r < bounds.y + bounds.height; r++)
      {
	for (c = bounds.x; c < bounds.x + bounds.width; c++)
	{
	  if (dungeon->hardness[r][c] == FLOOR_HARDNESS)
	  {
	    dungeon->terrain[r][c] = FLOOR;
	  }
	}	
      }
    }
    dungeon->pc = get_new_pc();
    point_t pc_loc = rect_center(dungeon->rooms[rand() % dungeon->num_rooms]);
    set_character_loc((character_t*)dungeon->pc, pc_loc);
    dungeon->characters[pc_loc.y][pc_loc.x] = ((character_t*)dungeon->pc);

    point_t up_stairs_loc = pc_loc;
    point_t down_stairs_loc;

    do
    {
      down_stairs_loc = rect_center(dungeon->rooms[rand() % dungeon->num_rooms]);
    } while (up_stairs_loc.x == down_stairs_loc.x && down_stairs_loc.y == down_stairs_loc.y);
    
    dungeon->terrain[up_stairs_loc.y][up_stairs_loc.x] = UP_STAIR;
    dungeon->terrain[down_stairs_loc.y][down_stairs_loc.x] = DOWN_STAIR;

    fclose(file); 
  }
  free(expanded);
  return dungeon;
}
