#include <core/game.h>
#include <core/entity.h>

#include "controller.h"

Controller::Controller() : Entity(nullptr)
{
	this->state = TANKBALL_STATE_INIT;
}

void Controller::update()
{
	bool sleep = true;
	for (Entity* obj : this->objects)
	{
		if (obj->isAwake())
		{
			sleep = false;
			break;
		}
	}

	if (sleep)
	{
		this->state |= TANKBALL_STATE_RUNNING;
	}
}

void Controller::next()
{
	if (this->state & TANKBALL_STATE_PLAYER)
	{
		this->state &= ~TANKBALL_STATE_PLAYER & 0xFF;
	}
	else
	{
		this->state |=  TANKBALL_STATE_PLAYER;
	}
}

uint8_t Controller::getPlayer()
{
	return !!(this->state & TANKBALL_STATE_PLAYER);
}
