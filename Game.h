#ifndef GAME_H
#define GAME_H

#define _USE_MATH_DEFINES

#define OLC_PGE_APPLICATION
#define OLC_PGEX_GRAPHICS2D
#define OLC_PGEX_SPLASHSCREEN

#include "olcPixelGameEngine.h"
#include "olcPGEX_SplashScreen.h"
#include "olcPGEX_Graphics2D.h"

#define OLC_SOUNDWAVE
#include "olcSoundWaveEngine.h"

#include "Level.h"
#include "Direction.h"

enum spell {
    GUST, SLEET, SUN
};

enum Weather {
    CLEAR, SUNNY, RAIN, SNOW
};

enum State
{
    ENEMY_TURN,
    LEVEL_END,
    PLAYER_INPUT,
    PLAYER_ACTION,
    GAME_LOSE,
    GAME_WIN
};

class Game : public olc::PixelGameEngine, olc::SplashScreen
{
private:
    int recharging = 0;

    void ProcessInput();

    float fCameraX = -32.0f;
    float fCameraY = -32.0f;

    std::vector<Level *> levels;
    Level* level;

    std::map<std::string, olc::Decal *> assets;
    std::map<std::string, olc::sound::Wave> sounds;

    olc::sound::WaveEngine engine;

    Weather weather = Weather::CLEAR;

    State state = State::GAME_LOSE;
    float delay = 0;
    bool switched_state = false;

    bool updateGameLose(float fElapsedTime);
    bool updateGameWin(float fElapsedTime);
    bool updatePlayerAction(float fElapsedTime);
    bool updatePlayerInput(float fElapsedTime);
    bool updateEnemyTurn(float fElapsedTime);
    bool updateLevelEnd(float fElapsedTime);

    void drawGame();
    void drawTiles();
    void drawObjects();
    void drawGui();
public:
    
    bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;

    std::map<int, olc::vi2d> mDirections;
    std::map<int, std::string> mWeather;

    int iPlayer;

    std::vector<std::string> spells;
    spell eSelectedSpell = spell::GUST;

    olc::Decal * getDecal(std::string name);

    Object* stormy;

    Game();
    ~Game();
};

#endif // GAME_H
