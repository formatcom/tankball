#ifndef _ENTITY_GROUND_H_
#define _ENTITY_GROUND_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <core/entity.h>

class Ground : public Entity
{
public:
	Ground(b2World * world);
	void render(SDL_Renderer *renderer);
};

#endif
