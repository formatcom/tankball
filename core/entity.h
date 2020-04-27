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

	// deberia pasar delta time
	virtual void update() {};
	virtual void render(SDL_Renderer *renderer) {};

	virtual b2Vec2   getPosition() { return this->body->GetPosition(); };
	virtual float32  getAngle()    { return this->body->GetAngle();    };
	virtual bool     isAwake()     { return this->body->IsAwake();     };

	// logger
	const char *name;
	int8_t slot;

protected:
	b2World       * world;

	b2BodyDef     * def;
	b2Body        * body;
	b2FixtureDef  * fixture;

	Color color;
};

#endif
