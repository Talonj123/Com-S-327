#ifndef _DICE_HPP_
#define _DICE_HPP_
#include <iostream>

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

#endif //_DICE_HPP_
