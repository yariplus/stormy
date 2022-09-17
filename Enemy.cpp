#include "Enemy.h"

Enemy::Enemy()
{
    Object();
};

Enemy::~Enemy()
{

}

int Enemy::getDelay()
{
    return delay;
}

void Enemy::decrementDelay()
{
    delay--;
}

void Enemy::resetDelay()
{
    delay = baseDelay;
}
