#include "items.hpp"
#include "generation.hpp"
#include "gameflow.h"

item::item(const object_data* source_data) : source(*source_data)
{

}

const object_data& item::get_source() const
{
  return source;
}

item_type item::get_type() const
{
  return type;
}

std::string item::get_name() const
{
  return name;
}

std::string item::get_description() const
{
  return description;
}

std::vector<int> item::get_colors() const
{
  return colors;
}

int item::get_hitpoints() const
{
  return hitpoints;
}

dice item::get_damage() const
{
  return damage;
}

int item::get_dodge() const
{
  return dodge;
}

int item::get_defense() const
{
  return defense;
}

int item::get_weight() const
{
  return weight;
}

int item::get_speed() const
{
  return speed;
}

int item::get_special() const
{
  return special;
}

int item::get_value() const
{
  return value;
}

char item::get_symbol() const
{
  return symbol;
}


void add_items(dungeon* dungeon, const std::vector<object_data> templates, int num_items)
{
  int i = 0;
  while (i < num_items)
  {
    object_data chosen_template = templates[rand() % templates.size()];
    if (!chosen_template.check_level(game_state.level))
    {
      continue;
    }
    i++;
    item* itm = chosen_template.create();
    int r, c;
    do
    {
      r = rand() % DUNGEON_ROWS;
      c = rand() % DUNGEON_COLS;
    } while (dungeon->terrain[r][c] != FLOOR ||
	     dungeon->items[r][c] != NULL);
    dungeon->items[r][c] = itm;
  }
}












































































