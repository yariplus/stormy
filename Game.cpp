#include "Game.h"
#include "Level.h"
#include "Direction.h"

int tilesize = 32;

Game::Game()
{
    sAppName = "Stormy";

    engine.InitialiseAudio();

    //sounds.emplace("Bang", olc::sound::Wave("./assets/music/SampleA.wav"));
    //sounds.emplace("Snow", olc::sound::Wave("./assets/music/Snowland Loop.wav"));
    sounds.emplace("Squish", olc::sound::Wave("./assets/sfx/impactsplat04.mp3.wav"));
}

Game::~Game()
{
}

bool Game::OnUserCreate()
{
    //engine.PlayWaveform(&sounds["Snow"], true);

    std::wstring lvl;
    Level* l;

    lvl =
        L"................"
        L"................"
        L".......@........"
        L"................"
        L"................"
        L"................"
        L".....M...M......"
        L".......S........"
        L".....S...S......"
        L".......S........"
        L"......SSS......."
        L".......S........"
        L"......MXM......."
        L".......M........"
        L"................"
        L"................";
    l = new Level(lvl, 16, 16);
    l->nextlvl = 1;
    levels.push_back(l);

    lvl =
        L"................"
        L"...........--..."
        L".......@..--...."
        L"................"
        L"BBBBBBBBBBBBBS.."
        L".....S..MM......"
        L".......MM..S...."
        L"...SBBBBBBBBBBBB"
        L"....M..........."
        L"...M............"
        L".M..---........."
        L"....-X-........."
        L"....---........."
        L"................"
        L"................"
        L"................";
    l = new Level(lvl, 16, 16);
    l->nextlvl = 2;
    levels.push_back(l);

    lvl =
        L"...M...@...M...."
        L"................"
        L"..------------.."
        L".M-----B------M."
        L"..------------.."
        L".M------------M."
        L"..--------B---.."
        L"..--B---------S."
        L".M------------M."
        L"..-B---xS---B-.."
        L".S--------B---.."
        L"..---B--------.."
        L".M------------M."
        L"..-----B------.."
        L"..------------.."
        L".....M.....M....";
    l = new Level(lvl, 16, 16);
    l->nextlvl = -1;
    levels.push_back(l);

    level = levels[0];

    stormy = new Object();
    stormy->setDecal("stormy");
    stormy->setPos(level->resetTiles());
    stormy->isTangable = false;

    iPlayer = level->nextObj;
    level->addObj(stormy);

    mDirections[0] = olc::vi2d(0, -1);
    mDirections[1] = olc::vi2d(0, 1);
    mDirections[2] = olc::vi2d(-1, 0);
    mDirections[3] = olc::vi2d(1, 0);
    
    mWeather[0] = "Clear";
    mWeather[1] = "Sunny";
    mWeather[2] = "Rain";
    mWeather[3] = "Snow";

    spells.push_back("Gust");
    spells.push_back("Sleet");
    spells.push_back("Sun");

    assets["stormy"] = new olc::Decal(new olc::Sprite("./assets/gfx/stormy.png"));
    assets["monster"] = new olc::Decal(new olc::Sprite("./assets/gfx/monster.png"));
    assets["spikes"] = new olc::Decal(new olc::Sprite("./assets/gfx/spikes.png"));
    assets["wall"] = new olc::Decal(new olc::Sprite("./assets/gfx/wall.png"));
    assets["exit"] = new olc::Decal(new olc::Sprite("./assets/gfx/exit.png"));
    assets["blood"] = new olc::Decal(new olc::Sprite("./assets/gfx/blood.png"));
    assets["skull"] = new olc::Decal(new olc::Sprite("./assets/gfx/skull.png"));

	return true;
}

bool Game::OnUserUpdate(float fElapsedTime)
{
    if (delay > 0) delay -= fElapsedTime;

    drawGame();

    switch (state)
    {
    case GAME_LOSE: return updateGameLose(fElapsedTime); break;
    case GAME_WIN: return updateGameWin(fElapsedTime); break;
    case PLAYER_INPUT: return updatePlayerInput(fElapsedTime); break;
    case PLAYER_ACTION: return updatePlayerAction(fElapsedTime); break;
    case ENEMY_TURN: return updateEnemyTurn(fElapsedTime); break;
    case LEVEL_END: return updateLevelEnd(fElapsedTime); break;
    }
}

void Game::drawGame()
{
    drawTiles();
    drawObjects();
    drawGui();
}

void Game::drawTiles()
{
    Clear(olc::Pixel(20, 20, 20));

    for (int x = 0; x < level->getWidth(); x++)
        for (int y = 0; y < level->getHeight(); y++)
        {
            Tile* tile = level->getTile(x, y);

            FillRect(x * tilesize - (int)fCameraX, y * tilesize - (int)fCameraY, tilesize, tilesize, tile->getColor());

            switch (weather)
            {
            case CLEAR:
                break;
            case SUNNY:
                if (0 == rand() % 50)
                {
                    if (tile->wet)
                    {
                        tile->wet = false;
                        tile->setColor(olc::Pixel(64, rand() % 12 + 150, 64));
                    }
                    if (tile->slippery)
                    {
                        tile->wet = true;
                        tile->setColor(olc::BLUE);
                    }
                }
                break;
            case RAIN:
                if (0 == rand() % 50)
                {
                    if (!tile->slippery)
                    {
                        tile->wet = true;
                        tile->setColor(olc::BLUE);
                    }
                }
                break;
            case SNOW:
                if (0 == rand() % 50)
                {
                    tile->wet = false;
                    tile->slippery = true;
                    tile->setColor(olc::WHITE);
                }
                break;
            }
        }
}

void Game::drawObjects()
{
    for (auto const& [key, object] : level->getObjects())
    {
        olc::vi2d pos = object->getPos();

        if (object->hasDecal)
        {
            DrawDecal({ (float)(object->getPos().x * tilesize - (int)fCameraX), (float)(object->getPos().y * tilesize - (int)fCameraY) }, getDecal(object->getDecal()), { 1.0f, 1.0f }, object->getTint());
        }

        Enemy* enemy = dynamic_cast<Enemy*>(object);
        if (enemy)
        {
            // Draw delay.
            DrawString({ (object->getPos().x * tilesize - (int)fCameraX), (object->getPos().y * tilesize - (int)fCameraY) }, std::to_string(enemy->getDelay()));
        }

        if (key == iPlayer)
        {
            FillRect(pos.x * tilesize - (int)fCameraX + (tilesize / 3), pos.y * tilesize - (int)fCameraY + (tilesize / 3), tilesize / 3, tilesize / 3, olc::MAGENTA);

            switch (object->getDirection())
            {
            case Direction::NORTH:
                DrawStringDecal({ pos.x * tilesize - fCameraX + 12, pos.y * tilesize - fCameraY + 0 }, "*");
                break;
            case Direction::SOUTH:
                DrawStringDecal({ pos.x * tilesize - fCameraX + 12, pos.y * tilesize - fCameraY + 25 }, "*");
                break;
            case Direction::EAST:
                DrawStringDecal({ pos.x * tilesize - fCameraX + 25, pos.y * tilesize - fCameraY + 12 }, "*");
                break;
            case Direction::WEST:
                DrawStringDecal({ pos.x * tilesize - fCameraX + 0, pos.y * tilesize - fCameraY + 12 }, "*");
                break;
            }
        }
    }
}

void Game::drawGui()
{
    DrawString(600, 70, "Spell:", olc::WHITE, 1);
    if (recharging > 0)
    {
        DrawString(600, 80, "Recharging", olc::WHITE, 2);
    }
    else {
        DrawString(600, 80, spells.at((int)eSelectedSpell), olc::WHITE, 2);
    }
}

bool Game::updatePlayerAction(float fElapsedTime) { return true; }

bool Game::updateGameLose(float fElapsedTime)
{
    if (delay <= 0)
    {
        state = State::PLAYER_INPUT;

        level = levels[0];
        stormy->setPos(level->resetTiles());
        iPlayer = level->nextObj;
        level->addObj(stormy);
    } else {
        DrawStringDecal({0.0f, 0.0f}, "You have died.");
    }

    return true;
}

bool Game::updateGameWin(float fElapsedTime)
{
    if (delay <= 0)
    {
        state = State::PLAYER_INPUT;

        level = levels[0];
        stormy->setPos(level->resetTiles());
        iPlayer = level->nextObj;
        level->addObj(stormy);
    }
    else {
        DrawStringDecal({ 0.0f, 0.0f }, "YOU WIN");
    }

    return true;
}

bool Game::updateLevelEnd(float fElapsedTime)
{
    if (delay <= 0)
    {
        state = State::PLAYER_INPUT;

        level = levels[level->nextlvl];
        stormy->setPos(level->resetTiles());
        iPlayer = level->nextObj;
        level->addObj(stormy);
    }
    else {
        DrawStringDecal({ 0.0f, 0.0f }, "Entering next level...");
    }

    return true;
}

bool Game::updateEnemyTurn(float fElapsedTime)
{
    for (auto const& [key, object] : level->getObjects())
    {
        olc::vi2d pos = object->getPos();

        Enemy* enemy = dynamic_cast<Enemy*>(object);
        if (enemy)
        {
            bool takenAction;

            if (0 == rand() % 10) continue;

            enemy->decrementDelay();

            if (enemy->getDelay() == 0)
            {
                enemy->resetDelay();

                // Add logic here...
                olc::vi2d dir = mDirections[rand() % 4];

                // Move it.
                takenAction = level->moveObjectCautiously(key, dir);

                if (takenAction)
                {
                    if (level->isPositionDeath(object->getPos()))
                    {
                        level->removeObj(key);
                        Object* blood = new Object();
                        blood->setDecal("blood");
                        blood->isLiving = false;
                        blood->isTangable = false;
                        blood->setPos(enemy->getPos());
                        level->addObj(blood);
                        engine.PlayWaveform(&sounds["Squish"]);
                    }

                    if (stormy->getPos() == enemy->getPos())
                    {
                        level->removeObj(iPlayer);
                        Object* skull = new Object();
                        skull->setDecal("skull");
                        skull->setPos(stormy->getPos());
                        level->addObj(skull);

                        delay = 5.0f;
                        state = State::GAME_LOSE;

                        return true;
                    }
                }
            }
        }
    }

    state = State::PLAYER_INPUT;

    return true;
}

bool Game::updatePlayerInput(float fElapsedTime)
{
    ProcessInput();

	return true;
}

void Game::ProcessInput()
{
    bool takenAction = false;

    Object* player = stormy;
    olc::vi2d ppos = player->getPos();
    olc::vi2d dir = mDirections[(int)player->getDirection()];

    if (GetKey(olc::Key::LEFT).bPressed) {
        if (player->getDirection() == Direction::WEST)
        {
            takenAction = level->moveObjectForcefully(iPlayer, dir);
        } else {
            player->setDirection(Direction::WEST);
        }
    } else if (GetKey(olc::Key::RIGHT).bPressed) {
        if (player->getDirection() == Direction::EAST)
        {
            takenAction = level->moveObjectForcefully(iPlayer, dir);
        } else {
            player->setDirection(Direction::EAST);
        }
    } else if (GetKey(olc::Key::UP).bPressed) {
        if (player->getDirection() == Direction::NORTH)
        {
            takenAction = level->moveObjectForcefully(iPlayer, dir);
        } else {
            player->setDirection(Direction::NORTH);
        }
    } else if (GetKey(olc::Key::DOWN).bPressed) {
        if (player->getDirection() == Direction::SOUTH)
        {
            takenAction = level->moveObjectForcefully(iPlayer, dir);
        } else {
            player->setDirection(Direction::SOUTH);
        }
    }
    else if (GetKey(olc::Key::SPACE).bPressed)
    {
        olc::vi2d target;

        if (recharging == 0)
        {
            switch (eSelectedSpell)
            {
            case SLEET:
                target = ppos + dir;

                for (int i = 0; i < 2; i++)
                {
                    if (level->isPositionValid(target))
                    {
                        level->getTile(target.x, target.y)->setColor(olc::WHITE);
                        level->getTile(target.x, target.y)->slippery = true;
                    }

                    target += dir;
                }

                recharging = 2;

                break;
            case GUST:
                target = ppos + dir * 2;

                for (int i = 0; i < 2; i++)
                {
                    if (level->isPositionValid(target))
                    {
                        std::unordered_set<int> objects = level->getTile(target.x, target.y)->objects;
                        for (auto obj : objects)
                        {
                            Object* o = level->getObject(obj);
                            if (dynamic_cast<Enemy*>(o))
                            {
                                //engine.PlayWaveform(&sounds["Bang"]);
                            }
                            if (level->getObject(obj)->pushable)
                            {
                                level->moveObjectForcefully(obj, dir);
                                int key = obj;
                                Object* object = level->getObject(obj);
                                if (level->isPositionDeath(object->getPos()) && object->isLiving)
                                {
                                    level->removeObj(key);
                                    Object* blood = new Object();
                                    blood->setDecal("blood");
                                    blood->isLiving = false;
                                    blood->isTangable = false;
                                    blood->setPos(object->getPos());
                                    level->addObj(blood);
                                    engine.PlayWaveform(&sounds["Squish"]);
                                }
                            }
                        }

                    }

                    target -= dir;
                }

                recharging = 2;

                break;
            case SUN:
                target = ppos + dir;

                for (int i = 0; i < 2; i++)
                {
                    if (level->isPositionValid(target))
                    {
                        level->getTile(target.x, target.y)->setColor(olc::Pixel(64, rand() % 12 + 150, 64));
                        level->getTile(target.x, target.y)->slippery = false;
                    }

                    target += dir;
                }

                recharging = 2;

                break;
            }

            takenAction = true;
        }
    } else if (GetKey(olc::Key::A).bPressed)
    {
        int iSelectedSpell = (int)eSelectedSpell + 1;
        iSelectedSpell = iSelectedSpell >= spells.size() ? 0 : iSelectedSpell;
        eSelectedSpell = static_cast<spell>(iSelectedSpell);
    }
    else if (GetKey(olc::Key::S).bPressed)
    {
        if (false)
        {
            int w = (int)weather;
            while (w == (int)weather)
            {
                w = rand() % 4;
            }
            weather = static_cast<Weather>(w);
        }
    }
    else if (GetKey(olc::Key::W).bPressed)
    {
        takenAction = true;
    }

    if (takenAction)
    {
        state = State::ENEMY_TURN;

        if (recharging > 0) recharging--;
    }

    // Handle win state.
    if (level->getTile(ppos)->exit)
    {
        if (level->nextlvl != -1)
        {
            delay = 3.0f;
            state = State::LEVEL_END;
        } else {
            delay = 5.0f;
            state = State::GAME_WIN;
        }
    }
    else if (level->isPositionDeath(ppos) || level->isPositionMonster(ppos))
    {
        level->removeObj(iPlayer);
        Object* skull = new Object();
        skull->setDecal("skull");
        skull->setPos(stormy->getPos());
        level->addObj(skull);

        delay = 5.0f;
        state = State::GAME_LOSE;
    }
}

olc::Decal * Game::getDecal(std::string name)
{
    return assets[name];
}

int main()
{
    Game stormy;

    if (stormy.Construct(768, 576, 2, 2)) stormy.Start();

    return 0;
}
