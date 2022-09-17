#ifndef LEVEL_H
#define LEVEL_H

#include "Tile.h"
#include "Object.h"
#include "Enemy.h"

#include <string>

class Level
{
public:
    Level(std::wstring sTiles, int width, int height);

    std::vector<Tile*> getTiles();

    Tile* getTile(int x, int y);
    Tile* getTile(olc::vi2d pos);
    void setTile(int x, int y, Tile* tile);

    olc::vi2d resetTiles();

    int getWidth();
    int getHeight();

    void addObj(Object* object);
    void removeObj(int key);
    Object* getObject(int obj);
    std::map<int, Object*> getObjects();

    bool moveObject(int key, olc::vi2d dir, bool force);
    bool moveObjectCautiously(int key, olc::vi2d dir);
    bool moveObjectForcefully(int key, olc::vi2d dir);

    bool isPositionValid(int x, int y);
    bool isPositionOpen(olc::vi2d pos);
    bool isPositionValid(olc::vi2d pos);
    bool isPositionDeath(olc::vi2d pos);

    bool isPositionMonster(olc::vi2d pos);

    int nextlvl = -1;
    int nextObj = 0;
private:
    std::wstring sBaseTiles;
    std::vector<Tile*> vTiles;

    int width;
    int height;

    std::map<int, Object*> objects;
};

#endif // LEVEL_H
