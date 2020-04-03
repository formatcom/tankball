#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include "color.h"

class Entity
{

public:
	Entity(b2World * world);
	virtual ~Entity();
	virtual void render(SDL_Renderer *renderer) = 0;

	virtual b2Vec2  getPosition() { return this->body->GetPosition(); };
	virtual float32 GetAngle()    { return this->body->GetAngle();    };

protected:
	b2World       * world;

	b2BodyDef     * def;
	b2Body        * body;
	b2FixtureDef  * fixture;

	Color color;
};

#endif
