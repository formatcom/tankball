#ifndef _ENTITY_LIMIT_H_
#define _ENTITY_LIMIT_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <core/entity.h>

class Limit : public Entity
{
public:
	Limit(b2World * world);
	void render(SDL_Renderer *renderer);
};

#endif
