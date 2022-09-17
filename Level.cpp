#include "Level.h"

Level::Level(std::wstring _tiles, int _width, int _height)
{
    this->sBaseTiles = _tiles;

    width = _width;
    height = _height;
}

std::vector<Tile*> Level::getTiles()
{
    return vTiles;
}

Tile* Level::getTile(int x, int y)
{
    return vTiles[x + y * width];
}

Tile* Level::getTile(olc::vi2d pos)
{
    return getTile(pos.x, pos.y);
}

void Level::setTile(int x, int y, Tile* tile)
{
    vTiles[x + y * width] = tile;
}

olc::vi2d Level::resetTiles()
{
    vTiles = std::vector<Tile*>();

    objects.clear();
    nextObj = 0;

    Object * obj = nullptr;

    olc::vi2d player_pos = olc::vi2d(0, 0);

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (x + y * width < sBaseTiles.size())
            {
                Tile* tile = new Tile(olc::WHITE);
                obj = nullptr;

                switch (sBaseTiles.at(x + y * width))
                {
                case 'S':
                    tile->setColor(olc::Pixel(64, rand() % 24 + 142, 64));
                    obj = new Object();
                    obj->setPos(olc::vi2d(x, y));
                    obj->isDeath = true;
                    obj->isLiving = false;
                    obj->isTangable = false;
                    obj->setDecal("spikes");
                    break;
                case 'M':
                    tile->setColor(olc::Pixel(64, rand() % 24 + 142, 64));
                    obj = new Enemy();
                    obj->setPos(olc::vi2d(x, y));
                    obj->setDecal("monster");
                    obj->pushable = true;
                    break;
                case 'B':
                    tile->setColor(olc::Pixel(64, 24, 24));
                    tile->solid = true;                    
                    obj = new Object();
                    obj->setPos(olc::vi2d(x, y));
                    obj->isLiving = false;
                    obj->setDecal("wall");
                    break;
                case 'X':
                    tile->setColor(olc::Pixel(64, rand() % 24 + 142, 64));
                    tile->exit = true;
                    obj = new Object();
                    obj->setPos(olc::vi2d(x, y));
                    obj->isLiving = false;
                    obj->isTangable = false;
                    obj->setDecal("exit");
                    break;
                case 'x':
                    tile->setColor(olc::WHITE);
                    tile->slippery = true;
                    tile->exit = true;
                    obj = new Object();
                    obj->setPos(olc::vi2d(x, y));
                    obj->isLiving = false;
                    obj->isTangable = false;
                    obj->setDecal("exit");
                    break;
                case '@':
                    player_pos = olc::vi2d(x, y);
                case '.':
                    tile->setColor(olc::Pixel(64, rand() % 12 + 150, 64));
                    break;
                case '-':
                    tile->setColor(olc::WHITE);
                    tile->slippery = true;
                    break;
                }

                vTiles.push_back(tile);
                if (obj) addObj(obj);
            }
            else
            {
                // Push empty space if size is wrong.
                vTiles.push_back(new Tile(olc::Pixel(64, rand() % 12 + 150, 64)));
            }
    return player_pos;
}

int Level::getWidth()
{
    return width;
}

int Level::getHeight()
{
    return height;
}

void Level::addObj(Object * object)
{
    objects[nextObj] = object;
    getTile(object->getPos())->addObject(nextObj);
    nextObj++;
}

void Level::removeObj(int key)
{
    Object* object = getObject(key);
    getTile(object->getPos())->removeObject(key);
    objects.erase(key);
}

Object* Level::getObject(int obj)
{
    return objects.at(obj);
}

std::map<int, Object*>  Level::getObjects()
{
    return objects;
}

bool Level::moveObjectCautiously(int key, olc::vi2d dir)
{
    Object* object = objects[key];
    olc::vi2d pos = object->getPos();

    if (isPositionValid(pos + dir) && isPositionOpen(pos + dir) && !isPositionDeath(pos + dir))
        return moveObject(key, dir, false);
    else
        return false;
}

bool Level::moveObjectForcefully(int key, olc::vi2d dir)
{
    Object* object = objects[key];
    olc::vi2d pos = object->getPos();

    if (isPositionValid(pos + dir) && !(isPositionMonster(pos) && isPositionMonster(pos + dir)))
        return moveObject(key, dir, false);
    else
        return false;
}

bool Level::moveObject(int key, olc::vi2d dir, bool force)
{
    Object* object = objects[key];
    olc::vi2d pos = object->getPos();

    if (!isPositionValid(pos + dir)) return false;

    do {
        pos += dir;
    } while (getTile(pos)->slippery && isPositionValid(pos + dir) && !isPositionDeath(pos) && !(!isPositionMonster(object->getPos()) && isPositionMonster(pos)) && !(isPositionMonster(object->getPos()) && isPositionMonster(pos + dir)));

    getTile(object->getPos())->removeObject(key);
    getTile(pos)->addObject(key);
    object->setPos(pos);

    return true;
}

bool Level::isPositionValid(int x, int y)
{
    return x < width && y < height && x >= 0 && y >= 0 && !getTile(x, y)->solid;
}

bool Level::isPositionOpen(olc::vi2d pos)
{
    bool isOpen = true;
    for (int o : getTile(pos)->objects)
    {
        if (getObject(o)->isTangable) isOpen = false;
    }
    return isOpen;
}

bool Level::isPositionValid(olc::vi2d pos)
{
    return isPositionValid(pos.x, pos.y);
}

bool Level::isPositionDeath(olc::vi2d pos)
{
    if (!isPositionValid(pos)) return false;

    bool isDeath = false;
    for (int o : getTile(pos)->objects)
    {
        if (getObject(o)->isDeath) isDeath = true;
    }
    return isDeath;
}

bool Level::isPositionMonster(olc::vi2d pos)
{
    if (!isPositionValid(pos)) return false;

    bool isDeath = false;
    for (int o : getTile(pos)->objects)
    {
        if (dynamic_cast<Enemy*>(getObject(o))) isDeath = true;
    }
    return isDeath;
}
