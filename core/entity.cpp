#include <stdlib.h>
#include <Box2D/Box2D.h>
#include "entity.h"


Entity::Entity(b2World * world) : world(world)
{
	this->def     = new b2BodyDef;
	this->fixture = new b2FixtureDef;
}

Entity::~Entity()
{
	free(this->def);
	free(this->body);
	free(this->fixture);
}
