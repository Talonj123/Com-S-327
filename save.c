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

#define TYPE rectangle_t
#define NAME room
#include "data_structures/list.h"

#define TYPE tile_t*
#define NAME tile
#include "data_structures/list.h"

#define TYPE tile_t*
#define NAME tile
#include "data_structures/queue.h"

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

rectangle_t get_bounds_rect(dungeon_t* dungeon, tile_t* center)
{
  rectangle_t bounds = {center->loc.x, center->loc.y, 1, 1};
  while (dungeon->tiles[center->loc.y][bounds.x].type == FLOOR)
  {
    bounds.x--;
    bounds.width++;
  }
  bounds.x++;
  while (dungeon->tiles[center->loc.y][bounds.x + bounds.width].type == FLOOR)
  {
    bounds.width++;
  }

  while (dungeon->tiles[bounds.y][center->loc.x].type == FLOOR)
  {
    bounds.y--;
    bounds.height++;
  }
  bounds.y++;

  while (dungeon->tiles[bounds.y + bounds.height][center->loc.x].type == FLOOR)
  {
    bounds.height++;
  }
  
  return bounds;
}

room_list_t* get_rooms(dungeon_t* dungeon)
{
  /* collect all the rooms, then find the bounds */
  tile_queue_t* centers = new_tile_queue();
  int r, c;
  for (r = 0; r < dungeon->rows; r++)
  {
    for (c = 0; c < dungeon->cols; c++)
    {
      if (dungeon->tiles[r][c].is_room_center)
      {
	tile_queue_enqueue(centers, &dungeon->tiles[r][c]);
      }
    }  
  }

  room_list_t* rooms = new_room_list();
  while (!tile_queue_is_empty(centers))
  {
    tile_t* center = tile_queue_dequeue(centers);
    rectangle_t bounds = get_bounds_rect(dungeon, center);
    room_list_add(rooms, bounds);
  }
  free(centers);
  return rooms;
}

char save_dungeon(dungeon_t* dungeon, char* name)
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

  room_list_t* rooms = get_rooms(dungeon);
  uint32_t size = 14 + (dungeon->rows*dungeon->cols) + (room_list_size(rooms)*4);
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
    fwrite(&version, 4, 1, file);

    size = htobe32(size);
    fwrite(&size, 4, 1, file);

    int r, c;
    for (r = 0; r < dungeon->rows; r++)
    {
      for (c = 0; c < dungeon->cols; c++)
      {
	char hardness = dungeon->tiles[r][c].hardness;
        fputc(hardness, file);
      }
    }
    int i;
    for (i = 0; i < room_list_size(rooms); i++)
    {
      rectangle_t room = room_list_get(rooms, i);
      char points[4];
      points[0] = room.x;
      points[1] = room.width;
      points[2] = room.y;
      points[3] = room.height;

      fwrite(points, 1, 4, file);
    }
    fclose(file);
    room_list_clean(rooms);
    free(rooms);
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
    dungeon->tiles = malloc(sizeof(tile_t*)*21);
    int r;
    for (r = 0; r < 21; r++)
    {
      dungeon->tiles[r] = malloc(sizeof(tile_t)*80);
    }
    dungeon->rows = 21;
    dungeon->cols = 80;
    set_tile_pointers(dungeon);
    /* dungeon allocated */
    for (r = 0; r < 21; r++)
    {
      int c;
      for (c = 0; c < 80; c++)
      {
	dungeon->tiles[r][c].loc.x = c;
	dungeon->tiles[r][c].loc.y = r;
	dungeon->tiles[r][c].hardness = fgetc(file);
	if (dungeon->tiles[r][c].hardness == 0)
	{
	  dungeon->tiles[r][c].type = HALL;
	}
	else 
	{
	  dungeon->tiles[r][c].type = WALL;

	}
      }
    }
    //print_hardnesses(dungeon);
    int i;
    tile_list_t* rooms = new_tile_list();
    for (i = 14 + 80*21; i < size; i+=4)
    {
      rectangle_t bounds;
      bounds.x = fgetc(file);
      bounds.width = fgetc(file);
      bounds.y = fgetc(file);
      bounds.height = fgetc(file);
      point_t center;
      center.x = rect_center_x(bounds);
      center.y = rect_center_y(bounds);
      dungeon->tiles[center.y][center.x].is_room_center = 1;
      tile_list_add(rooms, &dungeon->tiles[center.y][center.x]);
      int r, c;
      for (r = bounds.y; r < bounds.y + bounds.height; r++)
      {
	for (c = bounds.x; c < bounds.x + bounds.width; c++)
	{
	  if (dungeon->tiles[r][c].hardness == FLOOR_HARDNESS)
	  {
	    dungeon->tiles[r][c].type = FLOOR;
	  }
	}	
      }
    }
    dungeon->pc.loc = tile_list_get(rooms, rand() % tile_list_size(rooms))->loc;
    tile_list_clean(rooms);
    free(rooms);
    fclose(file); 
  }
  free(expanded);
  return dungeon;
}
