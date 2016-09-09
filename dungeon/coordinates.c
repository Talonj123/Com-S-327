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

int rect_center_x(rectangle_t rect)
{
  return rect.x + rect.width/2;
}

int rect_center_y(rectangle_t rect)
{
  return rect.y + rect.height/2;
}
