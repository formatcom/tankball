#ifndef _ENTITY_GROUND_H_
#define _ENTITY_GROUND_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <core/entity.h>

#include "controller.h"

class Ground : public Entity
{
public:
	Ground(Controller *controller, b2World * world);
	void render(SDL_Renderer *renderer);
	void update();

protected:
	void generate();

	Controller *controller;
};

#endif
