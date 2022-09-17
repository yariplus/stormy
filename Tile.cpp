#include "Tile.h"

Tile::Tile(olc::Pixel _color)
{
	color = _color;
}

olc::Pixel Tile::getColor()
{
	return color;
}

void Tile::setColor(olc::Pixel _color)
{
	color = _color;
}

void Tile::addObject(int obj)
{
	objects.insert(obj);
}

void Tile::removeObject(int obj)
{
	objects.erase(obj);
}
