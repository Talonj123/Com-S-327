#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include <stdio.h>

#include "dungeons_private.h"
#include "coordinates.h"

#define TYPE tile_t*
#define NAME tile
#include "data_structures/priority_queue.h"

#define TYPE tile_t*
#define NAME tile
#include "data_structures/list.h"

#define TYPE tile_t*
#define NAME tile
#include "data_structures/queue.h"

#define TYPE tile_dijkstra_t*
#define NAME path
#include "data_structures/priority_queue.h"

#define DIJKSTRA_TURN_COST (10)
#define DIJKSTRA_BASE_MOVE_COST (0)
#define DIJKSTRA_FLOOR_COST 30
#define DIJKSTRA_HALL_COST 5

/*
void print_null_tiles(dungeon_t* dungeon)
{
  int r, c;
  for (r = 0; r < 21; r++)
  {
    for (c = 0; c < 80; c++)
    {
      tile_t tile = dungeon->tiles[r][c];
      if (tile.right == NULL ||
	  tile.left == NULL ||
	  tile.up == NULL ||
	  tile.down == NULL)
      {
	printf("N");
      }
      else
      {
	printf(" ");
      }
    }
    printf("\n");
  }
  printf("\n");
}
*/

char check_join_domain(dungeon_t* dungeon, tile_queue_t* domains, point_t to_check, hardness_t to_join)
{ 
  tile_t* tile = &dungeon->tiles[to_check.y][to_check.x];
  if (tile->hardness == (MIN_HARDNESS + MAX_HARDNESS)/2)
  {
    tile->hardness = to_join;
    tile_queue_enqueue(domains, tile);
    return 1;
  }
  return 0;
}

char check_tile_join_domain(dungeon_t* dungeon, tile_queue_t* domains, tile_t* to_check, hardness_t to_join)
{
  if (to_check != NULL && to_check->hardness == (MIN_HARDNESS + MAX_HARDNESS)/2)
  {
    to_check->hardness = to_join;
    tile_queue_enqueue(domains, to_check);
    return 1;
  }
  return 0;
}

void diffuse(dungeon_t* dungeon, tile_queue_t* domains)
{
  while (!tile_queue_is_empty(domains))
  {
    tile_t* tile = tile_queue_dequeue(domains);
    /* part of a domain */
    /*
    point_t lloc = tile->loc;
    point_t rloc = tile->loc;
    point_t uloc = tile->loc;
    point_t dloc = tile->loc;
    
    lloc.x--;
    rloc.x++;
    uloc.y--;
    dloc.y++;
      
    check_join_domain(dungeon, domains, lloc, tile->hardness);
    check_join_domain(dungeon, domains, rloc, tile->hardness);
    check_join_domain(dungeon, domains, uloc, tile->hardness);
    check_join_domain(dungeon, domains, dloc, tile->hardness);
    */
    check_tile_join_domain(dungeon, domains, tile->left, tile->hardness);
    check_tile_join_domain(dungeon, domains, tile->right, tile->hardness);
    check_tile_join_domain(dungeon, domains, tile->up, tile->hardness);
    check_tile_join_domain(dungeon, domains, tile->down, tile->hardness);
  }
}

void soften(dungeon_t* dungeon)
{
//  rectangle_t border = {0, 0, dungeon->cols, dungeon->rows};
  int num_iterations = 80;
  int weight = 40;
  int i;
  for (i = 0; i < num_iterations; i++)
  {
    int r, c;
    for (r = 1; r < dungeon->rows - 1; r++)
    {
      for (c = 1; c < dungeon->cols - 1; c++)
      {
	tile_t* tile = &dungeon->tiles[r][c];
	/*
	point_t lloc = tile->loc;
	point_t rloc = tile->loc;
	point_t uloc = tile->loc;
	point_t dloc = tile->loc;
	
	lloc.x--;
	rloc.x++;
	uloc.y--;
	dloc.y++;
	
	int total_surrounding = 0;
	int denominator = 0;
	if (!rect_edge_contains_point(border, lloc))
	{
	  total_surrounding += dungeon->tiles[lloc.y][lloc.x].hardness;
	  denominator++;
	}
	if (!rect_edge_contains_point(border, rloc))
	{
	  total_surrounding += dungeon->tiles[rloc.y][rloc.x].hardness;
	  denominator++;
	}
	if (!rect_edge_contains_point(border, uloc))
	{
	  total_surrounding += dungeon->tiles[uloc.y][uloc.x].hardness;
	  denominator++;
	}
	if (!rect_edge_contains_point(border, dloc))
	{
	  total_surrounding += dungeon->tiles[dloc.y][dloc.x].hardness;
	  denominator++;
	}
	*/
	int total_surrounding = 0;
	int denominator = 0;
	if (tile->left != NULL)
	{
	  total_surrounding += tile->left->hardness;
	  denominator++;
	}
	if (tile->right != NULL)
	{
	  total_surrounding += tile->right->hardness;
	  denominator++;
	}
	if (tile->up != NULL)
	{
	  total_surrounding += tile->up->hardness;
	  denominator++;
	}
	if (tile->down != NULL)
	{
	  total_surrounding += tile->hardness;
	  denominator++;
	}
        double average_surrounding = ((double)total_surrounding)/denominator;
        double new_hardness = (tile->hardness * weight + average_surrounding)/(weight + 1);
        tile->hardness = (int)(new_hardness + 0.5);
      }
    }
  }
}

/* Generates a dungeon of the given size, filled with walls
 *   with random hardnesses, but the borders are 100% solid
 */
dungeon_t* get_blank_dungeon(int rows, int cols)
{
  /* freed by client */
  dungeon_t* dungeon = malloc(sizeof(dungeon_t));
  tile_queue_t* domains = new_tile_queue();
  dungeon->rows = rows;
  dungeon->cols = cols;
  dungeon->tiles = malloc(sizeof(tile_t*)*rows);
  int r, c;
  for (r = 0; r < rows; r++)
  {
    dungeon->tiles[r] = malloc(sizeof(tile_t)*cols);
    for (c = 0; c < cols; c++)
    {
      tile_t* tile = &dungeon->tiles[r][c];
      tile->loc.x = c;
      tile->loc.y = r;
      tile->type = WALL;
      /* Check for edge, if an edge tile, set to max hardness (100% hard)
           otherwise, use a random value (assumes that RAND_MAX >> MAX_HARDNESS)
      */
      if (r == 0 || c == 0 || r == (rows-1) || c == (cols-1))
      {
	tile->hardness = MAX_HARDNESS;
      }
      else
      {
	if (rand() % 4)
	{
	  /* should have a uniform distribution */
	  tile->hardness = (MIN_HARDNESS + MAX_HARDNESS)/2;
	}
	else
	{
	  /* should have a uniform distribution */
	  tile->hardness = MIN_HARDNESS + (rand() % (MAX_HARDNESS - MIN_HARDNESS));
          tile_queue_enqueue(domains, tile);
	}
      }
      tile->is_room_center = 0;
    }
  }
  for (r = 1; r < rows-1; r++)
  {
    for (c = 1; c < cols-1; c++)
    {
      dungeon->tiles[r][c].left = &dungeon->tiles[r][c-1];
      dungeon->tiles[r][c].right = &dungeon->tiles[r][c+1];
      dungeon->tiles[r][c].up = &dungeon->tiles[r-1][c];
      dungeon->tiles[r][c].down = &dungeon->tiles[r+1][c];
    }
  }
  for (r = 1; r < rows-1; r++)
  {
    dungeon->tiles[r][0].left = NULL;
    dungeon->tiles[r][0].right = &dungeon->tiles[r][1];
    dungeon->tiles[r][0].up = &dungeon->tiles[r-1][0];
    dungeon->tiles[r][0].down = &dungeon->tiles[r+1][0];

    dungeon->tiles[r][cols-1].left = &dungeon->tiles[r][rows-2];
    dungeon->tiles[r][cols-1].right = NULL;
    dungeon->tiles[r][cols-1].up = &dungeon->tiles[r-1][rows-1];
    dungeon->tiles[r][cols-1].down = &dungeon->tiles[r+1][rows-1];
  }
  for (c = 1; c < cols-1; c++)
  {
    dungeon->tiles[0][c].left = &dungeon->tiles[0][c-1];
    dungeon->tiles[0][c].right = &dungeon->tiles[0][c+1];
    dungeon->tiles[0][c].up = NULL;
    dungeon->tiles[0][c].down = &dungeon->tiles[1][c];

    dungeon->tiles[rows-1][c].left = &dungeon->tiles[rows-1][c-1];
    dungeon->tiles[rows-1][c].right = &dungeon->tiles[rows-1][c+1];
    dungeon->tiles[rows-1][c].up = &dungeon->tiles[rows-2][c];
    dungeon->tiles[rows-1][c].down = NULL;
  }

  dungeon->tiles[0][0].right = &dungeon->tiles[0][1];
  dungeon->tiles[0][0].down = &dungeon->tiles[1][0];
  dungeon->tiles[0][0].left = NULL;
  dungeon->tiles[0][0].up = NULL;

  dungeon->tiles[0][cols-1].right = NULL;
  dungeon->tiles[0][cols-1].down = &dungeon->tiles[1][cols-1];
  dungeon->tiles[0][cols-1].left = &dungeon->tiles[0][cols-2];
  dungeon->tiles[0][cols-1].up = NULL;

  dungeon->tiles[rows-1][cols-1].right = NULL;
  dungeon->tiles[rows-1][cols-1].down = NULL;
  dungeon->tiles[rows-1][cols-1].left = &dungeon->tiles[rows-1][cols-2];
  dungeon->tiles[rows-1][cols-1].up = &dungeon->tiles[rows-2][cols-1];

  dungeon->tiles[rows-1][0].right = &dungeon->tiles[rows-1][1];
  dungeon->tiles[rows-1][0].down = NULL;
  dungeon->tiles[rows-1][0].left = NULL;
  dungeon->tiles[rows-1][0].up = &dungeon->tiles[rows-2][0];

  diffuse(dungeon, domains);
  tile_queue_clear(domains);
  free(domains);
  soften(dungeon);
  return dungeon;
}

point_t room_center;
point_t scale;

int compare_tiles(tile_t* a, tile_t* b)
{
  int dx_a = (a->loc.x - room_center.x);
  int dy_a = (a->loc.y - room_center.y);
  
  int dx_b = (b->loc.x - room_center.x);
  int dy_b = (b->loc.y - room_center.y);

  double da = dx_a * dx_a + dy_a * dy_a;
  double db = dx_b * dx_b + dy_b * dy_b;

  da =  sqrt(da);
  db =  sqrt(db);

  if (abs(da-db) < .9)
  {
      return ((double)(a->hardness - b->hardness))/(10) + (da - db);
  }
  else
  {
    return (da - db);
  }
}

/* returns non-zero if the room colnd not be placed */
char place_random_room(dungeon_t* dungeon, rectangle_t border_rect)
{
  /* setup */
  int r, c;
  for (r = border_rect.y; r < border_rect.y + border_rect.height; r++)
  {
    for (c = border_rect.x; c < border_rect.x + border_rect.width; c++)
    {
	if ((dungeon->tiles[r][c].type != WALL) || (dungeon->tiles[r][c].hardness >= MAX_HARDNESS))
      {
	return 1;
      }
    }
  }
  /* Set the middle tile as open
   * then for each tile touching an open tile, randomly choose to remove open or not */
  room_center.x = rect_center_x(border_rect);
  room_center.y = rect_center_y(border_rect);
  tile_t* tile  = &dungeon->tiles[rect_center_y(border_rect)][rect_center_x(border_rect)];
  tile->type = FLOOR;
  tile->is_room_center = 1;
  tile->hardness = DIJKSTRA_FLOOR_COST;
  tile_pqueue_t* to_visit = new_tile_pqueue(compare_tiles);
  tile_pqueue_enqueue(to_visit, tile);
  while (!tile_pqueue_is_empty(to_visit))
  {
    tile = tile_pqueue_dequeue(to_visit);
    if (rect_edge_contains_point(border_rect, tile->loc))
    {
      break;
    }
    else
    {
      tile->type = FLOOR;
      tile->hardness = DIJKSTRA_FLOOR_COST;
    }
    /* now, add all the neighbors to the queue */
    point_t loc = tile->loc;
    /* right */
    point_t right = {loc.x+1, loc.y};
    if (rect_contains_point(border_rect, right))
    {
      tile_t* new_tile = &dungeon->tiles[right.y][right.x];
      if (new_tile->type == WALL)
      {
        tile_pqueue_enqueue(to_visit, new_tile);
      }
    }

    /* left */
    point_t left = {loc.x-1, loc.y};
    if (rect_contains_point(border_rect, left))
    {
      tile_t* new_tile = &dungeon->tiles[left.y][left.x];
      if (new_tile->type == WALL)
      {
        tile_pqueue_enqueue(to_visit, new_tile);
      }
    }

    /* up */
    point_t up = {loc.x, loc.y - 1};
    if (rect_contains_point(border_rect, up))
    {
      tile_t* new_tile = &dungeon->tiles[up.y][up.x];
      if (new_tile->type == WALL)
      {
        tile_pqueue_enqueue(to_visit, new_tile);
      }
    }

    /* down */
    point_t down = {loc.x, loc.y + 1};
    if (rect_contains_point(border_rect, down))
    {
      tile_t* new_tile = &dungeon->tiles[down.y][down.x];
      if (new_tile->type == WALL)
      {
        tile_pqueue_enqueue(to_visit, new_tile);
      }
    }
  }
  tile_pqueue_clear(to_visit);
  free(to_visit);
  return 0;
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
	if ((dungeon->tiles[r][c].type != WALL) || (dungeon->tiles[r][c].hardness >= MAX_HARDNESS))
      {
	return 1;
      }
    }
  }
  int buffer = 2;
  dungeon->tiles[border_rect.y + border_rect.height/2][border_rect.x + border_rect.width/2].is_room_center = 1;
  /* room can be created */
  for (r = border_rect.y + buffer; r < border_rect.y + border_rect.height - buffer; r++)
  {
    for (c = border_rect.x + buffer; c < border_rect.x + border_rect.width - buffer; c++)
    {
      dungeon->tiles[r][c].type = FLOOR;
      dungeon->tiles[r][c].hardness = DIJKSTRA_FLOOR_COST;
    }
  }
  
  return 0;
}

tile_list_t* find_rooms(dungeon_t* dungeon)
{
  tile_list_t* rooms = new_tile_list();

  int r, c;
  for (r = 0; r < dungeon->rows; r++)
  {
    for (c = 0; c < dungeon->cols; c++)
    {
      if (dungeon->tiles[r][c].is_room_center)
      {
	tile_list_add(rooms, &dungeon->tiles[r][c]);
      }
    }
  }

 return rooms;
}

int dijkstra_pqueue_compare(tile_dijkstra_t* a, tile_dijkstra_t* b)
{
  return (a->total_cost - b->total_cost) + (a->manhattan - b->manhattan);
}

double get_dist(tile_t* a, tile_t* b)
{
  int dx = a->loc.x - b->loc.x;
  int dy = a->loc.y - b->loc.y;
  double dd = dx*dx + dy*dy;
  return sqrt(dd);
}

void dijkstra_examine_neighbor(path_pqueue_t* to_visit, tile_dijkstra_t* parent, tile_dijkstra_t* neighbor, int new_cost)
{
  if (parent->parent != NULL)
  {
    point_t diff = {parent->tile->loc.x - parent->parent->tile->loc.x, 
		    parent->tile->loc.y - parent->parent->tile->loc.y};
  
    point_t ndiff = {neighbor->tile->loc.x - parent->tile->loc.x, 
		     neighbor->tile->loc.y - parent->tile->loc.y};
    if (diff.x != ndiff.x || diff.y != ndiff.y)
    {
      new_cost += DIJKSTRA_TURN_COST;
    }
  }
  if ((new_cost < neighbor->total_cost) && (neighbor->tile->hardness < MAX_HARDNESS))
  {
    /* set parent and score */ 
    neighbor->parent = parent;
    neighbor->total_cost = new_cost;
    path_pqueue_enqueue(to_visit, neighbor);
  }
}

void make_corridor(dungeon_t* dungeon, tile_t* room_a, tile_t* room_b)
{
  /* the list of nodes to check out, 'cheapest' first */
  path_pqueue_t* to_visit = new_path_pqueue(dijkstra_pqueue_compare);
  
  tile_dijkstra_t** tile_data = malloc(sizeof(tile_dijkstra_t*)*dungeon->rows);
  int r, c;
  for (r = 0; r < dungeon->rows; r++)
  {
    int malloc_size = sizeof(tile_dijkstra_t);
    malloc_size *= (dungeon->cols);
    tile_data[r] = malloc(malloc_size);
    for (c = 0; c < dungeon->cols; c++)
    {
      tile_data[r][c].tile = &dungeon->tiles[r][c];
      tile_data[r][c].total_cost = INT_MAX;
      tile_data[r][c].parent = NULL;
      int dx = abs(room_b->loc.x - c);
      int dy = abs(room_b->loc.y - r);
      tile_data[r][c].manhattan = dx + dy;
    }
  }
  
  tile_dijkstra_t* data = &tile_data[room_a->loc.y][room_a->loc.x];
  data->tile = room_a;
  data->total_cost = 0;
  path_pqueue_enqueue(to_visit, data);

  rectangle_t dungeon_bounds = {0, 0, dungeon->cols, dungeon->rows};
  while (!path_pqueue_is_empty(to_visit))
  {
    /* check out the tile's neighbors, and if the total cost to them reduces from what it was, add them to the queue */

    data = path_pqueue_dequeue(to_visit);
    if (data->tile == room_b)
    {
      /* target reached !!! */
      break;
    }
    point_t loc = data->tile->loc;
    int new_cost = data->total_cost + data->tile->hardness + DIJKSTRA_BASE_MOVE_COST;
    
    point_t rloc = loc, lloc = loc, uloc = loc, dloc = loc;
    rloc.x++;
    lloc.x--;
    uloc.y--;
    dloc.y++;

    if (rect_contains_point(dungeon_bounds, lloc))
    {
      tile_dijkstra_t* left = &tile_data[lloc.y][lloc.x];
      dijkstra_examine_neighbor(to_visit, data, left, new_cost);
    }
    if (rect_contains_point(dungeon_bounds, rloc))
    {
      tile_dijkstra_t* right = &tile_data[rloc.y][rloc.x];
      dijkstra_examine_neighbor(to_visit, data, right, new_cost);
    }
    if (rect_contains_point(dungeon_bounds, uloc))
    {
      tile_dijkstra_t* up = &tile_data[uloc.y][uloc.x];
      dijkstra_examine_neighbor(to_visit, data, up, new_cost);
    }
    if (rect_contains_point(dungeon_bounds, dloc))
    {
      tile_dijkstra_t* down = &tile_data[dloc.y][dloc.x];
      dijkstra_examine_neighbor(to_visit, data, down, new_cost);
    }
  }
  path_pqueue_clear(to_visit);
  free(to_visit);
  /* path found, backtrack and convert walls to halls */

  point_t loc = room_b->loc;
  tile_dijkstra_t* cell = &tile_data[loc.y][loc.x];
  while (cell != NULL)
  {
    if (cell->tile->type == WALL)
    {
      cell->tile->type = HALL;
      cell->tile->hardness = DIJKSTRA_FLOOR_COST;
    }

    cell = cell->parent;
  }

  for (r = 0; r < dungeon->rows; r++)
  {
  free(tile_data[r]);
  }
  free(tile_data);
}

void link_rooms(dungeon_t* dungeon)
{
  tile_list_t* rooms = find_rooms(dungeon);
  int iroom_a, iroom_b;
  for (iroom_a = 0; iroom_a < tile_list_size(rooms); iroom_a++)
  {
    tile_t* room_a = tile_list_get(rooms, iroom_a);
    for (iroom_b = 0; iroom_b < iroom_a; iroom_b++)
    {
      tile_t* room_b = tile_list_get(rooms, iroom_b);
      /* connect room_a to room_b */
      make_corridor(dungeon, room_a, room_b);
    }
  }
  tile_list_clean(rooms);
  free(rooms);
}

dungeon_t* get_dungeon(int rows, int cols, char rectangular)
{
  dungeon_t* dungeon;
  /* it is very uncommon to have < 7 rooms, so this loop should run twice at most */
  while (1)
  {
    dungeon = get_blank_dungeon(rows, cols);
    /* reduce for sparser dungeons */
    int max_fails = 50;
    int fails = 0;

    int i;

    rectangle_t outer_bounds = {1, 1, cols - 2, rows - 2};
    
    int min_width;
    int min_height;

    if (rectangular)
    {
      min_width = 8;
      min_height = 8;
    }
    else
    {
      min_width = 7;
      min_height = 7;
    }

    int max_width = cols/8;
    int max_height = rows/8;
    int rooms_created = 0;
    while (rooms_created < 8)
    {
      rectangle_t bounds;
      bounds.x = outer_bounds.x + (rand()%(outer_bounds.width-min_width));
      bounds.y = outer_bounds.y + (rand()%(outer_bounds.height-min_height));
      if (rectangular)
      {
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
      }
      else
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
      }

      if ((!rectangular && place_random_room(dungeon, bounds)) || 
	  (rectangular && place_rect_room(dungeon, bounds)))
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
	rooms_created++;
	fails = 0;
      }
    }	
    tile_list_t* rooms = find_rooms(dungeon);
    int num_rooms = tile_list_size(rooms);
    dungeon->pc.loc = tile_list_get(rooms, rand()%num_rooms)->loc;
    if (num_rooms >= 6)
    {
      tile_list_clean(rooms);
      free(rooms);
      break;
    }
    int r;
    for (r = 0; r < rows; r++)
    {
      free(dungeon->tiles[r]);
    }
    free(dungeon->tiles);
    free(dungeon);

    tile_list_clean(rooms);
    free(rooms);
  }

  link_rooms(dungeon);

  int r, c;
  for (r = 0; r < dungeon->rows; r++)
  {
    for (c = 0; c < dungeon->cols; c++)
    {
      if (dungeon->tiles[r][c].type == HALL)
      {
	dungeon->tiles[r][c].hardness = HALL_HARDNESS;
      }
      else if (dungeon->tiles[r][c].type == FLOOR)
      {
	dungeon->tiles[r][c].hardness = FLOOR_HARDNESS;
      }
    }
  }
  return dungeon;
}
