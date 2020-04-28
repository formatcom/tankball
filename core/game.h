#ifndef _CORE_GAME_H
#define _CORE_GAME_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "entity.h"

#define PTM              6.0
#define GRAVITY          9.8

#define STATE_INIT      (1<<0)
#define STATE_END       (1<<1)


#define MAX_CONTROLLERS    2

#define LED_RED   (1<<0)
#define LED_GREEN (1<<1)
#define LED_BLUE  (1<<2)

#define BUTTON_A     (1<<0)
#define BUTTON_LEFT  (1<<1)
#define BUTTON_RIGHT (1<<2)

class Game
{
public:

	static Game engine;

	Game(Game const&)            = delete;
	void operator=(Game const&)  = delete;

	int init(unsigned short width, unsigned short height, uint8_t framerate);
	void loop();
	void destroy(uint8_t withEntity);
	void destroy() { this->destroy(0); };
	void setFont( TTF_Font * f) { this->font = f; };
	TTF_Font * getFont() { return this->font; };

	uint8_t   getState()    { return this->state;   };
	uint16_t  getWidth()    { return this->width;   };
	uint16_t  getHeight()   { return this->height;  };

	uint8_t   getButtons(int8_t slot)               { return this->buttons[slot]; };
	SDL_GameController * getController(int8_t slot) { return this->controller[slot]; };

	std::vector<Entity*> objects;

	b2World * world;

protected:
	int  initSDL();             // Motor Grafico
	void initBox2D();           // Motor de fisicas

	void render();
	void update();

private:

	Game() {};
	~Game() {};


	// return -1 error
	int8_t slotController ();                    // return free slot controller
	int8_t slotController (SDL_JoystickID id);   // return      slot controller

	void addController    (int deviceIndex, int8_t slot, SDL_GameController *joy);
	void removeController (int8_t slot);

	void updateButton     (int8_t slot, uint8_t button, bool down);


	uint8_t state;

	uint16_t width;
	uint16_t height;

	uint8_t   framerate;
	uint32_t startTicks;
	uint32_t deltaTicks;
	TTF_Font *font;

	SDL_Window    *window;
	SDL_Renderer  *renderer;

	SDL_GameController *controller [MAX_CONTROLLERS];
	uint8_t             buttons    [MAX_CONTROLLERS];


};

#endif
