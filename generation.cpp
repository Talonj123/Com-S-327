//#define STANDALONE

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <ncurses.h>
#include "generation.hpp"

#define RepCheck(phase) if (found.phase) { error = true; break; } found.phase = true;

using namespace std;

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
      val += (rand() % sides) + 1;
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
  while (isspace(stream.peek()))
  {
    stream.get();
  }
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

bool operator>>(istream& stream, dice& d)
{
  return !dice::parse(stream, &d);
}

void dice::print(ostream& stream) const
{
  stream << base << '+' << num << 'd' << sides;
}

ostream& operator<<(ostream& stream, const dice& d)
{
  d.print(stream);
  return stream;
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
  BEGIN, DETAILS
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

  string next;
  bool complete = false;
  bool error = false;
  while (!complete && !error && (stream >> next))
  {
    switch (state)
    {
    case BEGIN:
      if (!next.compare("BEGIN"))
      {
	stream >> next;
	if (!next.compare("MONSTER"))
	{
	  state = DETAILS;
	}
      }
      break;
    case DETAILS:
      if (!next.compare("END"))
      {
	complete = true;
	break;
      }
      else if (!next.compare("NAME"))
      {
	RepCheck(name);
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	getline(stream, monster->name);
      }
      else if (!next.compare("SYMB"))
      {
	RepCheck(symbol);
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
      else if (!next.compare("COLOR"))
      {
	RepCheck(color);
	monster->colors.clear();
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	string colorname;
	while (stream.peek() != '\r' && stream.peek() != '\n')
	{
	  stream >> colorname;
	  int color = get_color(colorname);
	  if (color == -1)
	  {
	    error = true;
	    break;
	  }
	  else
	  {
	    monster->colors.push_back(color);
	  }
	}
	if (monster->colors.size() < 1)
	{
	  error = true;
	}
      }
      else if (!next.compare("DESC"))
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
      else if (!next.compare("SPEED"))
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
	error |= stream >> monster->speed;
      }
      else if (!next.compare("DAM"))
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
	error |= stream >> monster->damage;
      }
      else if (!next.compare("HP"))
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
	error |= stream >> monster->hitpoints;
      }
      else if (!next.compare("ABIL"))
      {
	if (found.abilities)
	{
	  error = true;
	  break;
	}
	found.abilities = true;
	monster->attributes.raw = 0;
	monster->attributes.corporeal = 1;
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

void monster_data::print(ostream& stream) const
{
  stream << "Name: " << name << endl;
  stream << "Symbol: " << symbol << endl;
  stream << "Description: " << description << endl;
  stream << "Colors:";

  for (vector<int>::const_iterator it = colors.begin() ; it != colors.end(); ++it)
  {
    cout << " " << get_color_name(*it);
  }
  cout << endl;

  stream << "Speed: " << speed << endl;

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
  stream << "Hitpoints: " << hitpoints << endl;
  stream << "Attack Damage: " << damage << endl;
}

ostream& operator<<(ostream& stream, const monster_data& mon)
{
  mon.print(stream);
  return stream;
}
#ifndef STANDALONE
monster* monster_data::create() const
{
  monster* monster = new ::monster();
  monster->symbol = symbol;
  monster->speed = speed;
  monster->type = MONSTER;
  monster->attributes = attributes;
  monster->colors = vector<int>(colors);
  monster->damage = damage;
  monster->hitpoints = monster->hitpoints_max = hitpoints;
  return monster;
}
#endif
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

int get_color(string name)
{
  if (!name.compare("BLACK"))
  {
    return COLOR_BLACK;
  }
  else if (!name.compare("RED"))
  {
    return COLOR_RED;
  }
  else if (!name.compare("GREEN"))
  {
    return COLOR_GREEN;
  }
  else if (!name.compare("YELLOW"))
  {
    return COLOR_YELLOW;
  }
  else if (!name.compare("BLUE"))
  {
    return COLOR_BLUE;
  }
  else if (!name.compare("MAGENTA"))
  {
    return COLOR_MAGENTA;
  }
  else if (!name.compare("CYAN"))
  {
    return COLOR_CYAN;
  }
  else if (!name.compare("WHITE"))
  {
    return COLOR_WHITE;
  }
  else
  {
    return -1;
  }
}

bool object_data::try_parse(std::istream& stream, object_data* object)
{
  
  stream.get();
  if (stream.eof())
  {
    return false;
  }
  stream.unget();
  enum {BEGIN, DETAILS} state = BEGIN;
  union
  {
    struct
    {
      bool name : 1;
      bool type : 1;
      bool color : 1;
      bool speed : 1;
      bool value : 1;
      bool dodge : 1;
      bool weight : 1;
      bool damage : 1;
      bool defense : 1;
      bool attribute : 1;
      bool hitpoints : 1;
      bool description : 1;
    };
    int raw;
  } found;
  found.raw = 0;
  bool complete = false, error = false;
  string next;
  while (!complete && !error && (stream >> next))
  {
    switch (state)
    {
    case BEGIN:
      if (!next.compare("BEGIN"))
      {
	stream >> next;
	if (!next.compare("OBJECT"))
	{
	  state = DETAILS;
	}
      }
      break;
    case DETAILS:
      if (!next.compare("END"))
      {
	complete = true;
	break;
      }
      else if (!next.compare("NAME"))
      {
	RepCheck(name);
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	getline(stream, object->name);
      }
      else if (!next.compare("TYPE"))
      {
	RepCheck(type);
	stream >> next;
	object->type = get_type(next);
      }
      else if (!next.compare("COLOR"))
      {
	RepCheck(color);
	object->colors.clear();
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	string colorname;
	while (stream.peek() != '\r' && stream.peek() != '\n')
	{
	  stream >> colorname;
	  int color = get_color(colorname);
	  if (color == -1)
	  {
	    error = true;
	    break;
	  }
	  else
	  {
	    object->colors.push_back(color);
	  }
	}
      }
      else if (!next.compare("WEIGHT"))
      {
	RepCheck(weight);
	error |= stream >> object->weight;
      }
      else if (!next.compare("HIT"))
      {
	RepCheck(hitpoints);
	error |= stream >> object->hitpoints;
      }
      else if (!next.compare("DAM"))
      {
	RepCheck(damage);
	error |= stream >> object->damage;
      }
      else if (!next.compare("ATTR"))
      {
	RepCheck(attribute);
	error |= stream >> object->special;
      }
      else if (!next.compare("VAL"))
      {
	RepCheck(value);
	error |= stream >> object->value;
      }
      else if (!next.compare("DODGE"))
      {
	RepCheck(dodge);
	error |= stream >> object->dodge;
      }
      else if (!next.compare("DEF"))
      {
	RepCheck(defense);
	error |= stream >> object->defense;
      }
      else if (!next.compare("SPEED"))
      {
	RepCheck(speed);
	error |= stream >> object->speed;
      }
      else if (!next.compare("DESC"))
      {
	RepCheck(description);
	while (isspace(stream.peek()))
	{
	  stream.get();
	}
	object->description.clear();
	char c;
	int length = 0;
	c = stream.get();
      PARSE_DESC_LINE:
	while (!(c == '\r' || c == '\n') && (length < 77))
	{
	  object->description += c;
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
	  object->description += ' ';
	  length = 0;
	  c = stream.get();
	  goto PARSE_DESC_LINE;
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
  if (found.raw != 0xFFF)
  {
    return false;
  }
  return complete;
}
#ifndef STANDALONE
item* object_data::create() const
{
  item* i = new item((object_data*)this);
  i->type = type;
  i->name = name;
  i->description = description;
  i->colors = colors;
  i->hitpoints = hitpoints;
  i->damage = damage;
  i->dodge = dodge;
  i->defense = defense;
  i->weight = weight;
  i->speed = speed;
  i->special = special;
  i->value = value;
  char table[] = { '|', ')', '}', '[', ']', '(', '{', '\\', '=', '\"', '_', '~', '?', '!', '$', '/', ',', '-', '%' };
  i->symbol = table[type];
  return i;
}
#endif
item_type object_data::get_type(string name)
{
  if (!name.compare("WEAPON")) return  WEAPON;
  if (!name.compare("OFFHAND")) return OFFHAND;
  if (!name.compare("RANGED")) return RANGED;
  if (!name.compare("ARMOR")) return ARMOR;
  if (!name.compare("HELMET")) return HELMET;
  if (!name.compare("CLOAK")) return CLOAK;
  if (!name.compare("GLOVES")) return GLOVES;
  if (!name.compare("BOOTS")) return BOOTS;
  if (!name.compare("RING")) return RING;
  if (!name.compare("AMULET")) return AMULET;
  if (!name.compare("LIGHT")) return LIGHT;
  if (!name.compare("SCROLL")) return SCROLL;
  if (!name.compare("BOOK")) return BOOK;
  if (!name.compare("FLASK")) return FLASK;
  if (!name.compare("GOLD")) return GOLD;
  if (!name.compare("AMMUNITION")) return AMMUNITION;
  if (!name.compare("FOOD")) return FOOD;
  if (!name.compare("WAND")) return WAND;
  if (!name.compare("CONTAINER")) return CONTAINER;
  return (item_type)-1;
}

string object_data::get_type_name(item_type type)
{
  switch (type)
  {
  case WEAPON: return "WEAPON";
  case OFFHAND: return "OFFHAND";
  case RANGED: return "RANGED";
  case ARMOR: return "ARMOR";
  case HELMET: return "HELMET";
  case CLOAK: return "CLOAK";
  case GLOVES: return "GLOVES";
  case BOOTS: return "BOOTS";
  case RING: return "RING";
  case AMULET: return "AMULET";
  case LIGHT: return "LIGHT";
  case SCROLL: return "SCROLL";
  case BOOK: return "BOOK";
  case FLASK: return "FLASK";
  case GOLD: return "GOLD";
  case AMMUNITION: return "AMMUNITION";
  case FOOD: return "FOOD";
  case WAND: return "WAND";
  case CONTAINER: return "CONTAINER";
  }
  return "????";
}

void object_data::print(ostream& stream) const
{
  stream << "Name: " << name << endl;
  stream << "Descritpion: " << description << endl;
  stream << "Type: " << get_type_name(type) << endl;
  stream << "Colors:";
  for (vector<int>::const_iterator it = colors.begin() ; it != colors.end(); ++it)
  {
    cout << " " << get_color_name(*it);
  }
  cout << endl;
  stream << "Hitpoint Bonus: " << hitpoints << endl;
  stream << "Damage Bonus: " << damage << endl;
  stream << "Dodge Bonus: " << dodge << endl;
  stream << "Defensive Bonus: " << defense << endl;
  stream << "Weight: " << weight << endl;
  stream << "Speed Bonus: " << speed << endl;
  stream << "Special: " << special << endl;
  stream << "Value: " << value << endl;
}

ostream& operator<<(ostream& stream, const object_data& obj)
{
  obj.print(stream);
  return stream;
}

vector<monster_data> read_monsters()
{
  ifstream file;
  string path(getenv("HOME"));
  path += "/.rlg327/monster_desc.txt";
  file.open(path.c_str());
  vector<monster_data> monster_types;
  monster_data mcurrent;
  while (monster_data::try_parse(file, &mcurrent))
  {
    monster_types.push_back(mcurrent);
  }
  file.close();
  return monster_types;
}

vector<object_data> read_objects()
{
  ifstream file;
  string path(getenv("HOME"));
  path += "/.rlg327/object_desc.txt";
  file.open(path.c_str());
  vector<object_data> object_types;
  object_data ocurrent;
  while (object_data::try_parse(file, &ocurrent))
  {
    object_types.push_back(ocurrent);
  }
  file.close();
  return object_types;
}
#ifdef STANDALONE
int main()
{
  ifstream file;
  string path(getenv("HOME"));
  path += "/.rlg327/monster_desc.txt";
  file.open(path.c_str());
  vector<monster_data> monster_types;
  monster_data mcurrent;
  while (monster_data::try_parse(file, &mcurrent))
  {
    monster_types.push_back(mcurrent);
  }
  for (vector<monster_data>::iterator it = monster_types.begin() ; it != monster_types.end(); ++it)
  {
    cout << endl;
    cout << *it << endl;
  }
  file.close();

  path = getenv("HOME");
  path += "/.rlg327/object_desc.txt";
  file.open(path.c_str());
  vector<object_data> object_types;
  object_data ocurrent;
  while (object_data::try_parse(file, &ocurrent))
  {
    object_types.push_back(ocurrent);
  }
  file.close();
  cout << object_types.size() << endl;
  for (vector<object_data>::iterator it = object_types.begin() ; it != object_types.end(); ++it)
  {
    //cout << endl;;
    //cout << *it << endl;
  }
}
#endif
