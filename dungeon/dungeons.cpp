//#include <stdlib>
#include <cmath>
#include <limits>
#include <cstdio>

#include <queue>

#include "dungeons_private.h"
#include "coordinates.h"
#include "characters.h"
#include "pathfinding.h"
#include "pqueue.hpp"

#define HARDNESS dungeon->hardness[r][c]
#define HARDNESS_AT(point) dungeon->hardness[point.y][point.x]
#define TERRAIN dungeon->terrain[r][c]
#define TERRAIN_AT(point) dungeon->terrain[point.y][point.x]
#define DISTANCE dungeon->distance_to_pc[r][c]
#define DISTANCE_AT(point) dungeon->distance_to_pc[point.y][point.x]
#define TUNNELING dungeon->tunneling_distance_to_pc[r][c]
#define TUNNELING_AT(point) dungeon->tunneling_distance_to_pc[point.y][point.x]
#define CHARACTER dungeon->characters[r][c]
#define CHARACTER_AT(point) dungeon->characters[point.y][point.x]
#define ITEM dungeon->items[r][c]
#define ITEM_AT(point) dungeon->items[point.y][point.x]

#define DIJKSTRA_TURN_COST (50)
#define DIJKSTRA_BASE_MOVE_COST (20)
#define DIJKSTRA_FLOOR_COST 20
#define DIJKSTRA_HALL_COST 5

#define NUM_ROOMS 10

using namespace std;

char check_join_domain(dungeon_t* dungeon, queue<point_t>& domains, point_t to_check, hardness_t  to_join)
{
  if (to_check.x < 0 || to_check.x >= DUNGEON_COLS || to_check.y < 0 || to_check.y >= DUNGEON_ROWS)
  {
    return 0;
  }
  if (HARDNESS_AT(to_check) == (MIN_HARDNESS + MAX_HARDNESS)/2)
  {
    HARDNESS_AT(to_check) = to_join;
    domains.push(to_check);
    return 1;
  }
  else
  {
    
  }
  return 0;
}

void diffuse(dungeon_t* dungeon, queue<point_t>& domains)
{
  while (!domains.empty())
  {
    point_t tile = domains.front();
    domains.pop();
    /* part of a domain */
    point_t lloc = tile;
    lloc.x--;
    point_t rloc = tile;
    rloc.x++;
    point_t uloc = tile;
    uloc.y--;
    point_t dloc = tile;
    dloc.y++;
    
    char hardness = HARDNESS_AT(tile);
    check_join_domain(dungeon, domains, lloc, hardness);
    check_join_domain(dungeon, domains, rloc, hardness);
    check_join_domain(dungeon, domains, uloc, hardness);
    check_join_domain(dungeon, domains, dloc, hardness);
  }
}

void soften(dungeon_t* dungeon)
{
  /* Higher makes it flatter */
  int num_iterations = 80;
  /* Higher makes it go flatter each iteration */
  int weight = 40;
  /* flatness scales in some manner with (iterations/weight) */
  int i;
  for (i = 0; i < num_iterations; i++)
  {
    int r, c;
    for (r = 1; r < DUNGEON_ROWS - 1; r++)
    {
      for (c = 1; c < DUNGEON_COLS - 1; c++)
      {
	int total_surrounding = 0;
	int denominator = 0;
	/* left */
	if (c - 1 > 1)
	{
	  total_surrounding += dungeon->hardness[r][c - 1];
	  denominator++;
	}
	/* right */
	if (c + 1 < DUNGEON_ROWS - 1)
	{
	  total_surrounding += dungeon->hardness[r][c + 1];
	  denominator++;
	}
	/* up */
	if (r - 1 > 1)
	{
	  total_surrounding += dungeon->hardness[r - 1][c];
	  denominator++;
	}
	/* down */
	if (r + 1 < DUNGEON_ROWS - 1)
	{
	  total_surrounding += dungeon->hardness[r + 1][c];
	  denominator++;
	}
        double average_surrounding = ((double)total_surrounding)/denominator;
        double new_hardness = (HARDNESS * weight + average_surrounding)/(weight + 1);
        HARDNESS = (int)(new_hardness + 0.5);
      }
    }
  }
}

/* Generates a dungeon of the given size, filled with walls
 *   with random hardnesses, but the borders are 100% solid
 */
dungeon_t* get_blank_dungeon()
{
  /* freed by client */
  dungeon_t* dungeon = (dungeon_t*)malloc(sizeof(dungeon_t));
  dungeon->num_rooms = 0;
  dungeon->num_characters = 0;
  dungeon->rooms = NULL;
  dungeon->pc = NULL;

  queue<point_t> domains;

  int r, c;
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      TERRAIN = WALL;
      DISTANCE = numeric_limits<int>::max();
      TUNNELING = numeric_limits<int>::max();
      CHARACTER = NULL;
      ITEM = NULL;
      /* Check for edge, if an edge tile, set to max hardness (100% hard, 0% breakable)
           otherwise, use a random value (assumes that RAND_MAX >> MAX_HARDNESS)
      */
      if (r == 0 || c == 0 || r == (DUNGEON_ROWS-1) || c == (DUNGEON_COLS-1))
      {
        HARDNESS = MAX_HARDNESS;
      }
      else
      {
	if (rand() % 10)
	{
	  /* should have a uniform distribution */
	  HARDNESS = (MIN_HARDNESS + MAX_HARDNESS)/2;
	}
	else
	{
	  do
	  {
	  /* should have a uniform distribution */
	  HARDNESS = MIN_HARDNESS + (rand() % (MAX_HARDNESS - MIN_HARDNESS));
	  } while (HARDNESS == (MAX_HARDNESS+MIN_HARDNESS)/2 || HARDNESS == MAX_HARDNESS || HARDNESS == MIN_HARDNESS);
	  point_t domain = {c, r};
          domains.push(domain);
	}
      }
    }
  }

  diffuse(dungeon, domains);

  soften(dungeon);
  return dungeon;
}

/* returns non-zero if the room colnd not be placed */
char place_rect_room(dungeon_t* dungeon, rectangle_t border_rect)
{
  /* setup */
  int r, c;
  for (r = border_rect.y; r < border_rect.y + border_rect.height; r++)
  {
    for (c = border_rect.x; c < border_rect.x + border_rect.width; c++)
    {
      if ((TERRAIN != WALL) || (HARDNESS >= MAX_HARDNESS))
      {
	return 1;
      }
    }
  }
  /* the number of rows to leave blank */
  int buffer = 2;
  /* room can be created */
  for (r = border_rect.y + buffer; r < border_rect.y + border_rect.height - buffer; r++)
  {
    for (c = border_rect.x + buffer; c < border_rect.x + border_rect.width - buffer; c++)
    {
      TERRAIN = FLOOR;
      HARDNESS = 0;
    }
  }
  
  return 0;
}

bool dijkstra_pqueue_compare(tile_dijkstra_t* a, tile_dijkstra_t* b)
{
  return (a->total_cost + a->manhattan) > (b->total_cost + b->manhattan);
}

double get_dist(point_t a, point_t b)
{
  int dx = a.x - b.x;
  int dy = a.y - b.y;
  double dd = dx*dx + dy*dy;
  return sqrt(dd);
}

void dijkstra_examine_neighbor(dungeon_t* dungeon, PriorityQueue<tile_dijkstra_t*>& to_visit, tile_dijkstra_t* parent, tile_dijkstra_t* neighbor, int new_cost)
{
  if (parent->parent != NULL)
  {
    point_t diff = {parent->tile.x - parent->parent->tile.x, 
		    parent->tile.y - parent->parent->tile.y};
  
    point_t ndiff = {neighbor->tile.x - parent->tile.x, 
		     neighbor->tile.y - parent->tile.y};
    if (diff.x != ndiff.x || diff.y != ndiff.y)
    {
      new_cost += DIJKSTRA_TURN_COST;
    }
  }
  if ((new_cost < neighbor->total_cost) && (HARDNESS_AT(neighbor->tile) < MAX_HARDNESS))
  {
    /* set parent and score */ 
    neighbor->parent = parent;
    neighbor->total_cost = new_cost;
    to_visit.enqueue(neighbor);
  }
}

void make_corridor(dungeon_t* dungeon, point_t room_a, point_t room_b)
{
  /* the list of nodes to check out, 'cheapest' first */
  PriorityQueue<tile_dijkstra_t*> to_visit(dijkstra_pqueue_compare);
  
  tile_dijkstra_t tile_data[DUNGEON_ROWS][DUNGEON_COLS];
  int r, c;
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      point_t t = {c, r};
      tile_data[r][c].tile = t;
      tile_data[r][c].total_cost = numeric_limits<int>::max();
      tile_data[r][c].parent = NULL;
      int dx = abs(room_b.x - c);
      int dy = abs(room_b.y - r);
      tile_data[r][c].manhattan = dx + dy;
    }
  }
  
  tile_dijkstra_t* data = &tile_data[room_a.y][room_a.x];
  data->tile = room_a;
  data->total_cost = 0;
  to_visit.enqueue(data);

  while (!to_visit.empty())
  {
    /* check out the tile's neighbors, and if the total cost to them reduces from what it was, add them to the queue */

    data = to_visit.dequeue();
    if (data->tile.x == room_b.x &&
	data->tile.y == room_b.y)
    {
      /* target reached !!! */
      break;
    }
    point_t loc = data->tile;
    int new_cost = data->total_cost + DIJKSTRA_BASE_MOVE_COST;

    switch (TERRAIN_AT(data->tile))
    {
    case FLOOR:
      new_cost += DIJKSTRA_FLOOR_COST;
      break;
    case HALL:
      new_cost += DIJKSTRA_HALL_COST;
      break;
    case WALL:
      new_cost += HARDNESS_AT(data->tile);
      break;
    case UP_STAIR:
      new_cost += HARDNESS_AT(data->tile);
      break;
    case DOWN_STAIR:
      new_cost += HARDNESS_AT(data->tile);
      break;
    }
    if (loc.x > 0)
    {
      tile_dijkstra_t* tile = &tile_data[loc.y][loc.x - 1];
      dijkstra_examine_neighbor(dungeon, to_visit, data, tile, new_cost);
    }
    if (loc.x < DUNGEON_COLS - 1)
    {
      tile_dijkstra_t* tile = &tile_data[loc.y][loc.x + 1];
      dijkstra_examine_neighbor(dungeon, to_visit, data, tile, new_cost);
    }
    if (loc.y > 0)
    {
      tile_dijkstra_t* tile = &tile_data[loc.y - 1][loc.x];
      dijkstra_examine_neighbor(dungeon, to_visit, data, tile, new_cost);
    }
    if (loc.y < DUNGEON_ROWS)
    {
      tile_dijkstra_t* tile = &tile_data[loc.y + 1][loc.x];
      dijkstra_examine_neighbor(dungeon, to_visit, data, tile, new_cost);
    }
  }

  /* path found, backtrack and convert walls to halls */

  point_t loc = room_b;
  tile_dijkstra_t* cell = &tile_data[loc.y][loc.x];
  while (cell != NULL)
  {
    if (TERRAIN_AT(cell->tile) == WALL)
    {
      TERRAIN_AT(cell->tile) = HALL;
      HARDNESS_AT(cell->tile) = FLOOR_HARDNESS;
    }

    cell = cell->parent;
  }
}

void link_rooms(dungeon_t* dungeon)
{
  int iroom_a, iroom_b;
  for (iroom_a = 0; iroom_a < dungeon->num_rooms; iroom_a++)
  {
    rectangle_t room_a = dungeon->rooms[iroom_a];
    for (iroom_b = 0; iroom_b < iroom_a; iroom_b++)
    {
      rectangle_t room_b = dungeon->rooms[iroom_b];
      /* connect room_a to room_b */
      make_corridor(dungeon, rect_center(room_a), rect_center(room_b));
    }
  }
}

dungeon_t* dungeon_new()
{
  dungeon_t* dungeon;
  /* it is very uncommon to have < 7 rooms, so this loop should run twice at most */
  while (1)
  {
    //TODO:
    dungeon = get_blank_dungeon();
    /* reduce for sparser dungeons */
    int max_fails = 500;
    int fails = 0;

    int i;

    rectangle_t outer_bounds = {1, 1, DUNGEON_COLS - 2, DUNGEON_ROWS - 2};

    int min_width = 8;
    int min_height = 8;

    int max_width = DUNGEON_COLS/6;
    int max_height = DUNGEON_ROWS/6;
    int rooms_created = 0;
    dungeon->num_rooms = NUM_ROOMS;
    dungeon->rooms = (rectangle_t*)malloc(sizeof(rectangle_t)*NUM_ROOMS);

    while (rooms_created < NUM_ROOMS)
    {
      rectangle_t bounds;
      bounds.x = outer_bounds.x + (rand()%(outer_bounds.width-min_width));
      bounds.y = outer_bounds.y + (rand()%(outer_bounds.height-min_height));

      /* limit aspect ratio of rooms */
      double ar;
      do
      {
	int mwidth = (outer_bounds.x + outer_bounds.width - bounds.x - min_width);
	if (mwidth > max_width)
	{
	  mwidth = max_width;
	}
	int mheight = (outer_bounds.y + outer_bounds.height - bounds.y - min_height);
	if (mheight > max_height)
	{
	  mheight = max_height;
	}
	bounds.width = min_width + (rand() % mwidth);
	bounds.height = min_height + (rand() % mheight);
	
	if (bounds.width > bounds.height)
	{
	  ar = (bounds.width-4)/(double)(bounds.height-4); 
	}
	else
	{
	  ar = (bounds.height-4)/((double)bounds.width-4); 
	}
      } while (ar >= 1.5);
      
      if (place_rect_room(dungeon, bounds))
      {
	i--;
	fails++;
	if (fails >= max_fails)
	{
	  break;
	}
      }
      else
      {
	rectangle_t actual = {bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4};
	dungeon->rooms[rooms_created++] = actual;
	fails = 0;
      }
    }

    point_t up_stairs_loc = rect_center(dungeon->rooms[rand() % rooms_created]);
    point_t down_stairs_loc;

    do
    {
      down_stairs_loc = rect_center(dungeon->rooms[rand() % rooms_created]);
    } while (up_stairs_loc.x == down_stairs_loc.x && down_stairs_loc.y == down_stairs_loc.y);

    TERRAIN_AT(up_stairs_loc) = UP_STAIR;
    TERRAIN_AT(down_stairs_loc) = DOWN_STAIR;
    
    if (rooms_created >= NUM_ROOMS)
    {
      break;
    }
    dungeon_free(dungeon);
  }

  link_rooms(dungeon);
 
 return dungeon;
}
void dungeon_free(dungeon_t* dungeon)
{
  int r, c;
  for (r = 1; r < DUNGEON_ROWS - 1; r++)
  {
    for (c = 1; c < DUNGEON_COLS - 1; c++)
    {
      if (CHARACTER != NULL && CHARACTER->type != PC)
      {
	free_character(CHARACTER);
      }
    }
  }
  free(dungeon->rooms);
  free(dungeon);
}

void set_pc(dungeon* dungeon, player* pc)
{
  if (dungeon->pc != NULL)
  {
    dungeon->num_characters--;
    free_pc(dungeon->pc);
  }  

  dungeon->pc = pc;
  dungeon->num_characters++;

  int r, c;
  point_t pc_loc;
  for (r = 0; r < DUNGEON_ROWS; r++)
  {
    for (c = 0; c < DUNGEON_COLS; c++)
    {
      if (dungeon->terrain[r][c] == UP_STAIR)
      {
	pc_loc.x = c;
	pc_loc.y = r;
	goto BREAK;
      }
    }
  }
 BREAK:
  set_character_loc((character*)pc, pc_loc);
  CHARACTER_AT(pc_loc) = ((character*)dungeon->pc);

  get_distances(dungeon);
}
