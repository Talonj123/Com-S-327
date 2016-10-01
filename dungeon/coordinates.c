#include "coordinates.h"


char rect_contains_point(rectangle_t rect, point_t point)
{
  return (point.x >= rect.x && point.x < rect.x + rect.width) &&
         (point.y >= rect.y && point.y < rect.y + rect.height);
}

char rect_edge_contains_point(rectangle_t rect, point_t point)
{
  return ((point.x == rect.x || point.x == rect.x + rect.width - 1)
	  && (point.y >= rect.y && point.y <= rect.y + rect.height)) ||
         ((point.y == rect.y || point.y == rect.y + rect.height - 1)
	  && (point.x >= rect.x && point.x <= rect.x + rect.width));
}

point_t rect_center(rectangle_t rect)
{
  point_t ret = {rect.x + rect.width/2, rect.y + rect.height/2};
  return ret;
}

int rect_center_x(rectangle_t rect)
{
  return rect.x + rect.width/2;
}

int rect_center_y(rectangle_t rect)
{
  return rect.y + rect.height/2;
}

char rect_equals(rectangle_t a, rectangle_t b)
{
  return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

char point_equals(point_t a, point_t b)
{
  return a.x == b.x && a.y == b.y;
}
