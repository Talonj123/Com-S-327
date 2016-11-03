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
  void print(std::ostream&);
  operator int () const;

  static bool parse(std::istream&, dice*);
};

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

  void print(std::ostream&);

};

#endif //_GENERATION_H_
