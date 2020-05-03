#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <core/game.h>
#include <core/entity.h>

#include "controller.h"

Controller::Controller() : Entity(nullptr)
{
	this->color.rgba = 0xFFFF00FF;
	this->state = TANKBALL_STATE_INIT;
}

void Controller::notify(SDL_Renderer *renderer, const char *text)
{
	SDL_Surface* surface = TTF_RenderText_Solid(
								Game::engine.getFont(),
								text,
								*(SDL_Color*)(&this->color));

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

}

void Controller::render(SDL_Renderer *renderer)
{
	if (this->state & TANKBALL_STATE_INFO && ~this->state & TANKBALL_STATE_RECV)
	{
		char buf[10];
		sprintf(buf, "player %d", this->getPlayer()+1);
		this->notify(renderer, buf);
	}
}

void Controller::update()
{

	if (this->state & TANKBALL_STATE_RESTART)
	{
		this->state = TANKBALL_STATE_INIT;
	}

	uint8_t buttons = Game::engine.getButtons(this->slot);

	if (buttons & BUTTON_A)
	{
		this->step++;

		if (this->step >= Game::engine.getFrameRate())
		{
			this->step = 0;
			this->time++;
		}

		// RESTART GAME
		if (this->time >= 10) // 10 seg
		{
			this->state = TANKBALL_STATE_RESTART;

			this->step = 0;
			this->time = 0;
		}

	}


	bool sleep = true;
	for (Entity* obj : this->objects)
	{
		if (obj->isAwake() && obj->active)
		{
			sleep = false;
			break;
		}
	}

	if (sleep)
	{
		this->state |= TANKBALL_STATE_RUNNING|TANKBALL_STATE_INFO;
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

	this->state &= ~TANKBALL_STATE_RUNNING & 0xFF;
	this->state &= ~TANKBALL_STATE_INFO    & 0xFF;
	this->state &= ~TANKBALL_STATE_RECV    & 0xFF;
}

uint8_t Controller::getPlayer()
{
	return !!(this->state & TANKBALL_STATE_PLAYER);
}
