#ifndef _GENERATION_H_
#define _GENERATION_H_

typedef class monster_data monster_data;
typedef class object_data object_data;

#include "characters.h"
#include "items.hpp"

class dice
{
private:
  int base;
  int num;
  int sides;

public:
  dice(int base = 0, int num = 0, int sides = 0);
  int roll() const;
  operator int () const;

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
  monster* create() const;

  void print(std::ostream&) const;

};

std::ostream& operator<<(std::ostream&, const monster_data&);

class object_data
{
private:
  std::string name;
  std::string description;
  item_type type;
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
  item* create() const;
  static bool try_parse(std::istream&, object_data*);
  static item_type get_type(std::string name);
  static std::string get_type_name(item_type type);

  void print(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&, const object_data&);

int get_color(std::string name);
std::string get_color_name(int color);

std::vector<monster_data> read_monsters();
std::vector<object_data> read_objects();

#endif //_GENERATION_H_
