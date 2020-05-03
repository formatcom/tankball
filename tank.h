#ifndef _ENTITY_TANK_H_
#define _ENTITY_TANK_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <core/entity.h>
#include "controller.h"
#include "bullet.h"

class Tank : public Entity
{
public:
	Tank(Controller *controller, Bullet *bullet, b2World * world);
	void update();
	void render(SDL_Renderer *renderer);

	void contact(Entity *entityA, Entity *entityB);

protected:
	int16_t     radius;
	Bullet     *bullet;
	Controller *controller;

	uint8_t step; // frame

	uint8_t lastButtons;
};

#endif
