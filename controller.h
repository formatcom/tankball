#ifndef _ENTITY_CONTROLLER_H_
#define _ENTITY_CONTROLLER_H_

#include <vector>
#include <core/entity.h>

#define TANKBALL_STATE_INIT    (   0)
#define TANKBALL_STATE_RUNNING (1<<1)
#define TANKBALL_STATE_PLAYER  (1<<2)

class Controller : public Entity
{
public:
	Controller();
	void    update();
	void    next();
	uint8_t getPlayer();

	bool    isAwake()     { return false; };

	uint8_t state;
	std::vector<Entity*> objects;
};

#endif
