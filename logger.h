#ifndef _ENTITY_LOGGER_H_
#define _ENTITY_LOGGER_H_

#include <vector>
#include <core/entity.h>

#include "controller.h"

class Logger : public Entity
{
public:
	Logger(Controller *controller);
	void render(SDL_Renderer *renderer);

	std::vector<Entity*> objects;

	bool    isAwake()     { return false; };

private:

	void print   (SDL_Renderer *renderer, const char *text, int x, int y);
	void println (SDL_Renderer *renderer, const char *text);

	Controller *controller;

	int   line;
	int   size;
};

#endif
