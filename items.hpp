#ifndef _ITEMS_HPP_
#define _ITEMS_HPP_

#include <cstdlib>
#include <string>
#include <vector>

typedef class object_data object_data;

typedef enum item_type
{WEAPON, OFFHAND, RANGED, ARMOR, HELMET, CLOAK, GLOVES, BOOTS, RING, AMULET, LIGHT, SCROLL, BOOK, FLASK, GOLD, AMMUNITION, FOOD, WAND, CONTAINER} item_type;

class item
{
  friend object_data;
private:
  const object_data& source;
  item_type type;
  std::string name;
  std::string description;
  std::vector<int> colors;
  int hitpoints;
  int damage;
  int dodge;
  int defense;
  int weight;
  int speed;
  int special;
  int value;
  char symbol;

  item(const object_data*);
public:
  const object_data& get_source() const;
  item_type get_type() const;
  std::string get_name() const;
  std::string get_description() const;
  std::vector<int> get_colors() const;
  int get_hitpoints() const;
  int get_damage() const;
  int get_dodge() const;
  int get_defense() const;
  int get_weight() const;
  int get_speed() const;
  int get_special() const;
  int get_value() const;
  char get_symbol() const;

};

#endif //_ITEMS_HPP_
