#ifndef _ENTITY_BULLET_H_
#define _ENTITY_BULLET_H_

#include <Box2D/Box2D.h>
#include <core/entity.h>

class Bullet
{
public:
	Bullet(b2World * world);

	void shot(b2Vec2 position, float angle, uint8_t power);

	Entity *entity [3];
	uint8_t len;
};

#endif
