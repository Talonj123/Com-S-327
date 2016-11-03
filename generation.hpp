#ifndef _GENERATION_H_
#define _GENERATION_H_

#include "characters.h"

class dice
{
private:
  int base;
  int num;
  int sides;

public:
  dice(int base = 0, int num = 0, int sides = 0);
  int roll() const;
  //operator int () const;

  void print(std::ostream&) const;

  static bool parse(std::istream&, dice*);
};

std::ostream& operator<<(std::ostream&, const dice&);

class monster_data
{
private:
  std::string name;
  char symbol;
  std::string description;
  std::vector<int> colors;
  dice speed;
  monster_attributes attributes;
  dice hitpoints;
  dice damage;

public:
  monster_data() : symbol('?') {};
  static bool try_parse(std::istream&, monster_data*);
  Monster create() const;

  void print(std::ostream&) const;

};

class object_data
{
public:
  typedef enum object_type
  {WEAPON, OFFHAND, RANGED, ARMOR, HELMET, CLOAK, GLOVES, BOOTS, RING, AMULET, LIGHT, SCROLL, BOOK, FLASK, GOLD, AMMUNITION, FOOD, WAND, CONTAINER} type;
private:
  std::string name;
  std::string description;
  type item_type;
  std::vector<int> colors;
  dice hitpoints;
  dice damage;
  dice dodge;
  dice defense;
  dice weight;
  dice speed;
  dice special;
  dice value;

public:
  static bool try_parse(std::istream&, object_data*);
  static object_data::type get_type(std::string name);
  static std::string get_type_name(object_data::type type);

  void print(std::ostream&) const;
};


int get_color(std::string name);
std::string get_color_name(int color);

#endif //_GENERATION_H_
