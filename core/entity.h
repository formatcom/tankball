#ifndef _CORE_ENTITY_H_
#define _CORE_ENTITY_H_

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

	virtual uint8_t  getType()     { return this->type;               };

	virtual b2Body * getBody()     { return this->body; };
	virtual b2Vec2   getPosition() { return this->body->GetPosition(); };
	virtual float32  getAngle()    { return this->body->GetAngle();    };
	virtual bool     isAwake()     { return this->body->IsAwake();     };

	virtual void     setColor(uint32_t color) { this->color.rgba = color; };

	virtual void     contact(Entity *entityA, Entity *entityB) {};

	// logger
	const char *name;
	int8_t slot;
	int8_t life;
	int8_t power;
	bool   active;

protected:
	b2World       * world;

	b2BodyDef     * def;
	b2Body        * body;
	b2FixtureDef  * fixture;

	uint8_t type;

	Color color;
};

#endif
