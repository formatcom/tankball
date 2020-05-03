#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <Box2D/Box2D.h>
#include <core/game.h>
#include <core/entity.h>

#include "logger.h"


Logger::Logger(Controller *controller) : Entity(nullptr)
{
	this->controller = controller;
	this->line =  0;
	this->size = 16;
	this->color.rgba = 0xFFFFFFFF;
}

void Logger::print(SDL_Renderer *renderer, const char *text, int x, int y)
{
	SDL_Rect position;
	SDL_Surface* surface = TTF_RenderText_Solid(
								Game::engine.getFont(),
								text,
								*(SDL_Color*)(&this->color));

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	position.x =   x;
	position.y =   y;
	position.w = surface->w;
	position.h = surface->h;

	SDL_RenderCopy(renderer, texture, NULL, &position);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void Logger::println(SDL_Renderer *renderer, const char *text)
{
	this->print(renderer, text, 0, this->line*this->size);
	this->line++;
}


void Logger::render(SDL_Renderer *renderer)
{
	this->line = 0; // reset line for loop

	this->println(renderer, "tankball logger");
	this->println(renderer, " ");

	char buf[250];
	for (int8_t slot = 0; slot < MAX_CONTROLLERS; ++slot)
	{
		SDL_GameController *joy = Game::engine.getController(slot);
		sprintf(buf, "slot[%d] controller %25s %X",
				slot,
				SDL_GameControllerName(joy),
				Game::engine.getButtons(slot));
		this->println(renderer, buf);
	}

	this->println(renderer, " ");

	uint8_t i = 0;
	char turn[100];
	for (Entity* obj : this->objects)
	{

		if (i == this->controller->getPlayer())
		{
			if (this->controller->state & TANKBALL_STATE_SET_ANGLE)
				sprintf(turn, "* angle");
			else if (this->controller->state & TANKBALL_STATE_SET_POWER)
				sprintf(turn, "* power");
			else
				sprintf(turn, "* move ");
		}
		else
		{
				sprintf(turn, " ");
		}

		b2Vec2 position = obj->getPosition();
		sprintf(buf, "%s slot %d life %d awake %d %08.02f %08.02f %03d %s",
				obj->name,
				obj->slot,
				obj->life,
				obj->isAwake(),
				position.x,
				position.y,
				obj->power,
				turn);
		this->println(renderer, buf);
		i++;
	}

}
