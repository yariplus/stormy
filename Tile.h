#ifndef TILE_H
#define TILE_H

#include "olcPixelGameEngine.h"

#include <string>
#include <unordered_set>

class Tile
{
public:
    Tile(olc::Pixel color);
    olc::Pixel getColor();
    void setColor(olc::Pixel _color);

    bool slippery = false;
    bool wet = false;
    bool solid = false;
    bool exit = false;

    void addObject(int obj);
    void removeObject(int obj);

    std::unordered_set<int> objects;
private:
    olc::Pixel color;

    
};

#endif // TILE_H
