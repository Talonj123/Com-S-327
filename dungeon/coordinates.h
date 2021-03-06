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

point_t rect_center(rectangle_t rect);
int rect_center_x(rectangle_t rect);
int rect_center_y(rectangle_t rect);

char rect_equals(rectangle_t a, rectangle_t b);
char point_equals(point_t a, point_t b);

#endif
