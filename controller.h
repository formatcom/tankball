#ifndef _ENTITY_CONTROLLER_H_
#define _ENTITY_CONTROLLER_H_

#include <vector>
#include <core/entity.h>

#define TANKBALL_STATE_INIT        (   0)
#define TANKBALL_STATE_RUNNING     (1<<1)
#define TANKBALL_STATE_PLAYER      (1<<2)
#define TANKBALL_STATE_SET_MOVE    (1<<3)
#define TANKBALL_STATE_SET_ANGLE   (1<<4)
#define TANKBALL_STATE_SET_POWER   (1<<5)
#define TANKBALL_STATE_INFO        (1<<6)
#define TANKBALL_STATE_RECV        (1<<7)


class Controller : public Entity
{
public:
	Controller();
	void    update();
	void    render(SDL_Renderer *renderer);
	void    next();
	uint8_t getPlayer();

	bool    isAwake()     { return false; };

	uint8_t state;
	std::vector<Entity*> objects;

	void notify(SDL_Renderer *renderer, const char *text);
};

#endif
