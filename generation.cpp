#include <cstdlib>
#include <tr1/regex>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <ncurses.h>
#include "generation.hpp"

using namespace std;
using namespace std::tr1;

dice::dice(int base, int num, int sides)
{
  this->base = base;
  this->num = num;
  this->sides = sides;
}

int dice::roll() const
{
  int val = base;
  if (sides > 0)
  {
    int i;
    for (i = 0; i < num; i++)
    {
      val += rand() % sides;
    }
  }
  return val;
}

dice::operator int () const
{
  return roll();
}

bool dice::parse(istream& stream, dice* target)
{
  int value = 0;
  while (isdigit(stream.peek()))
  {
    value *= 10;
    value += stream.get() - '0';
  }
  int base = value;
  if (stream.get() != '+')
  {
    return false;
  }
  value = 0;
  while (isdigit(stream.peek()))
  {
    value *= 10;
    value += stream.get() - '0';
  }
  int num = value;
  if (stream.get() != 'd')
  {
    return false;
  }
  value = 0;
  while (isdigit(stream.peek()))
  {
    value *= 10;
    value += stream.get() - '0';
  }
  int sides = value;
  *target = dice(base, num, sides);
  return true;
}

void dice::print(std::ostream& stream)
{
  stream << base << '+' << num << 'd' << sides << endl;
}

bool monster_data::try_parse(istream& stream, monster_data* monster)
{
 MONSTER_DATA_PARSE_TOP:
  stream.get();
  if (stream.eof())
  {
    return false;
  }
  stream.unget();
  enum
  {
  BEGIN, MONSTER, DETAILS
  } state;
  state = BEGIN;

  union
  {
    struct
    {
      bool name : 1;
      bool speed : 1;
      bool color : 1;
      bool symbol : 1;
      bool damage : 1;
      bool abilities : 1;
      bool hitpoints : 1;
      bool description : 1;
    };
    int raw;
  } found;
  found.raw = 0;

  string line;
  bool complete = false;
  bool error = false;
  while (!complete && !error && (stream >> line))
  {
    switch (state)
    {
    case BEGIN:
      if (!line.compare("BEGIN"))
      {
	state = MONSTER;
      }
      break;
    case MONSTER:
      if (!line.compare("MONSTER"))
      {
	state = DETAILS;
      }
      break;
    case DETAILS:
      if (!line.compare("END"))
      {
	complete = true;
	break;
      }
      else if (!line.compare("NAME"))
      {
	if (found.name)
	{
	  error = true;
	  break;
	}
	found.name = true;
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	getline(stream, monster->name);
      }
      else if (!line.compare("SYMB"))
      {
	if (found.symbol)
	{
	  error = true;
	  break;
	}
	found.symbol = true;
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	stream >> monster->symbol;
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
      }
      else if (!line.compare("COLOR"))
      {
	if (found.color)
	{
	  error = true;
	  break;
	}
	found.color = true;
	monster->colors.clear();
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	string colorname;
	while (stream.peek() != '\r' && stream.peek() != '\n')
	{
	  stream >> colorname;
	  if (!colorname.compare("BLACK"))
	  {
	    monster->colors.push_back(COLOR_BLACK);
	  }
	  else if (!colorname.compare("RED"))
	  {
	    monster->colors.push_back(COLOR_RED);
	  }
	  else if (!colorname.compare("GREEN"))
	  {
	    monster->colors.push_back(COLOR_GREEN);
	  }
	  else if (!colorname.compare("YELLOW"))
	  {
	    monster->colors.push_back(COLOR_YELLOW);
	  }
	  else if (!colorname.compare("BLUE"))
	  {
	    monster->colors.push_back(COLOR_BLUE);
	  }
	  else if (!colorname.compare("MAGENTA"))
	  {
	    monster->colors.push_back(COLOR_MAGENTA);
	  }
	  else if (!colorname.compare("CYAN"))
	  {
	    monster->colors.push_back(COLOR_CYAN);
	  }
	  else if (!colorname.compare("WHITE"))
	  {
	    monster->colors.push_back(COLOR_WHITE);
	  }
	  else
	  {
	    error = true;
	  }
	}
      }
      else if (!line.compare("DESC"))
      {
	if (found.description)
	{
	  error = true;
	  break;
	}
	found.description = true;
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	monster->description.clear();
	char c;
	int length = 0;
	c = stream.get();
      PARSE_DESC_LINE:
	while (!(c == '\r' || c == '\n') && (length < 77))
	{
	  monster->description += c;
	  length++;
	  c = stream.get();
	}
	if (stream.peek() == '\n')
	{
	  stream.get();
	}
	if (length > 77)
	{
	  error = true;
	}
	else
	{
	  // line termineated before limit
	  if (stream.peek() == '.')
	  {
	    stream.get();
	    if (stream.peek() == '\r' || stream.peek() == '\n')
	    {
	      stream.unget();
	      break;
	    }
	  }
	  monster->description += ' ';
	  length = 0;
	  c = stream.get();
	  goto PARSE_DESC_LINE;
	}
      }
      else if (!line.compare("SPEED"))
      {
	if (found.speed)
	{
	  error = true;
	  break;
	}
	found.speed = true;
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	error |= !dice::parse(stream, &monster->speed);
      }
      else if (!line.compare("DAM"))
      {
	if (found.damage)
	{
	  error = true;
	  break;
	}
	found.damage = true;
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	error |= !dice::parse(stream, &monster->damage);
      }
      else if (!line.compare("HP"))
      {
	if (found.hitpoints)
	{
	  error = true;
	  break;
	}
	found.hitpoints = true;
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	error |= !dice::parse(stream, &monster->hitpoints);
      }
      else if (!line.compare("ABIL"))
      {
	if (found.abilities)
	{
	  error = true;
	  break;
	}
	found.abilities = true;
        while (stream.peek() != '\r' && stream.peek() != '\n')
	{
	  monster->attributes.corporeal = 1;
	  string ability;
	  stream >> ability;
	  if (!ability.compare("SMART"))
	  {
	    monster->attributes.intelligent = 1;
	  }
	  else if (!ability.compare("TELE"))
	  {
	    monster->attributes.telepathic = 1;
	  }
	  else if (!ability.compare("TUNNEL"))
	  {
	    monster->attributes.tunneling = 1;
	  }
	  else if (!ability.compare("ERRATIC"))
	  {
	    monster->attributes.erratic = 1;
	  }
	  else if (!ability.compare("PASS"))
	  {
	    monster->attributes.corporeal = 0;
	  }
	  else
	  {
	    error = true;
	  }
	}
      }
      break;
    }
    if (error)
    {
      error = false;
      complete = false;
      found.raw = 0;
      state = BEGIN;
      if (stream.eof())
      {
	return false;
      }
    } 
  }
  if (found.raw != 0xFF)
  {
    goto MONSTER_DATA_PARSE_TOP;
  }
  return complete;
}

string  get_color_name(int color)
{
  if (color == COLOR_RED) return "RED";
  if (color == COLOR_BLACK) return "BLACK";
  if (color == COLOR_GREEN) return "GREEN";
  if (color == COLOR_YELLOW) return "YELLOW";
  if (color == COLOR_BLUE) return "BLUE";
  if (color == COLOR_MAGENTA) return "MAGENTA";
  if (color == COLOR_CYAN) return "CYAN";
  if (color == COLOR_WHITE) return "WHITE";
  return "????";
}

void monster_data::print(ostream& stream)
{
  stream << "Name: " << name << endl;
  stream << "Symbol: " << symbol << endl;
  stream << "Description: " << description << endl;
  stream << "Colors:";

  for (vector<int>::iterator it = colors.begin() ; it != colors.end(); ++it)
  {
    cout << " " << get_color_name(*it);
  }
  cout << endl;

  stream << "Speed: ";
  speed.print(stream);

  stream << "Abilities:";
  if (attributes.intelligent)
  {
      stream << " intelligent";
  }
  if (attributes.telepathic)
  {
      stream << " telepathic";
  }
  if (attributes.tunneling)
  {
      stream << " tunneling";
  }
  if (attributes.erratic)
  {
      stream << " erratic";
  }
  if (!attributes.corporeal)
  {
      stream << " non-corporeal";
  }
  stream << endl;
  stream << "Hitpoints: ";
  hitpoints.print(stream);
  stream << "Attack Damage: ";
  damage.print(stream);
}

int main()
{
  monster_data data;
  ifstream file;
  string path(getenv("HOME"));
  path += "/.rlg327/monster_desc.txt";
  cout << path << endl;
  file.open(path.c_str());
  vector<monster_data> monster_types;
  monster_data current;
  while (monster_data::try_parse(file, &current))
  {
    monster_types.push_back(current);
    cout << "loaded monster # " << monster_types.size() << endl;
  }
  file.close();
  for (vector<monster_data>::iterator it = monster_types.begin() ; it != monster_types.end(); ++it)
  {
    cout << endl << endl;
    it->print(cout);
  }
}
