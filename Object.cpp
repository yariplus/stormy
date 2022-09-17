#include "Object.h"

Object::Object()
{
	pos = olc::vi2d(0, 0);
	direction = Direction::SOUTH;
}

olc::vi2d Object::getPos()
{
	return pos;
}

void Object::setPos(olc::vi2d _pos)
{
	pos = _pos;
}

Direction Object::getDirection()
{
	return direction;
}

void Object::setDirection(Direction _direction)
{
	direction = _direction;
}

olc::Pixel Object::getTint()
{
	return tint;
}

void Object::setTint(olc::Pixel _tint)
{
	tint = _tint;
}

void Object::setDecal(std::string name)
{
	decal = name;
	hasDecal = true;
}

std::string Object::getDecal()
{
	return decal;
}
