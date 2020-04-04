#ifndef _GAME_H
#define _GAME_H

#include <vector>
#include <SDL2/SDL.h>
#include "entity.h"

#define PTM              6.0
#define GRAVITY          9.8
#define STATE_INIT      1<<0
#define STATE_END       1<<1


#define MAX_CONTROLLERS    2

#define LED_RED   (1<<0)
#define LED_GREEN (1<<1)
#define LED_BLUE  (1<<2)

class Game
{
public:

	static Game engine;

	Game(Game const&)            = delete;
	void operator=(Game const&)  = delete;

	int init(unsigned short width, unsigned short height, uint8_t framerate);
	void loop();
	void destroy(uint8_t withEntity);
	void destroy() { this->destroy(0); } ;

	uint8_t   buttons;
	uint8_t   Lastbuttons;

	uint8_t   getState()  { return this->state;  };
	uint16_t  getWidth()  { return this->width;  };
	uint16_t  getHeight() { return this->height; };

	std::vector<Entity*> objects;

	b2World * world;

protected:
	int  initSDL();             // Motor Grafico
	void initBox2D();           // Motor de fisicas

	void render();

private:

	Game() {};
	~Game() {};

	uint8_t state;

	uint16_t width;
	uint16_t height;

	uint8_t   framerate;
	uint32_t startTicks;
	uint32_t deltaTicks;

	SDL_Window    *window;
	SDL_Renderer  *renderer;

	uint8_t indexController;
	SDL_GameController *controller [MAX_CONTROLLERS];

};

#endif
