#ifndef _TYPES_H_ 
#define _TYPES_H_

typedef struct point_t
{
  int x, y;
} point_t;

typedef struct rectangle_t
{
  int x, y, width, height;
} rectangle_t;

char rect_contains_point(rectangle_t rect, point_t point);
char rect_edge_contains_point(rectangle_t rect, point_t point);
int rect_center_x(rectangle_t rect);
int rect_center_y(rectangle_t rect);

#endif
