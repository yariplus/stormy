#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include "Object.h"

enum Type
{
	NORMAL,
	FAST,
	STRONG,
	STICKY,
	JUMP,
	SPLIT,
	BRIGHT,
	GROUNDED
};

class Enemy : public Object
{
public:
	Enemy();
	~Enemy();

	int getDelay();
	void setDelay(int delay);
	void addDelay(int delay);
	void decrementDelay();
	void resetDelay();

	int baseDelay = 3;

	Type type = NORMAL;
private:
	int delay = 3;
};

#endif // ENEMY_H
