#ifndef _ENTITY_TANK_H_
#define _ENTITY_TANK_H_

#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include <core/entity.h>

class Tank : public Entity
{
public:
	Tank(b2World * world);
	void render(SDL_Renderer *renderer);
	void setColor(uint32_t color) { this->color.rgba = color; };

protected:
	int16_t radius;
};

#endif
