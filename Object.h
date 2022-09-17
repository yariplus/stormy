#ifndef OBJECT_H
#define OBJECT_H

#include "olcPixelGameEngine.h"

#include "Direction.h"

class Object
{
public:
    Object();

    olc::vi2d getPos();
    void setPos(olc::vi2d);

    Direction getDirection();
    void setDirection(Direction _direction);

    olc::Pixel getTint();
    void setTint(olc::Pixel _tint);

    bool hasDecal = false;
    std::string getDecal();
    void setDecal(std::string);

    bool pushable = false;
    bool isTangable = true;
    bool isLiving = true;
    bool isDeath = false;

    virtual void a() {}
private:
    olc::vi2d pos;

    std::string decal = "";

    olc::Pixel tint = olc::WHITE;

    Direction direction;
};

#endif // OBJECT_H
