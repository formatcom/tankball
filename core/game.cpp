#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include "hack_SDL.h"
#include "game.h"

Game Game::engine;

int Game::initSDL()
{
	this->window   = NULL;
	this->renderer = NULL;

	if (SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER ) != 0)
	{
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(this->width, this->height,
				SDL_WINDOW_SHOWN, &this->window, &this->renderer))
	{
		return 2;
	}

	return 0;
}

void Game::initBox2D()
{
	this->world = new b2World(b2Vec2(0.0, GRAVITY));
}

int Game::init(uint16_t width, uint16_t height, uint8_t framerate)
{

	srand(time(NULL));

	this->state     = STATE_INIT;
	this->width     = width;
	this->height    = height;

	this->framerate = framerate;

	this->initSDL();

	this->initBox2D();

	return 0;
}

void Game::render()
{

	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(this->renderer);

	for (Entity* obj : this->objects)
	{
		obj->render(this->renderer);
	}

	SDL_RenderPresent(this->renderer);
}

void Game::loop()
{
	SDL_Event event;

	while((this->state & STATE_END) == 0)
	{
		while(SDL_PollEvent(&event) == 1)
		{
			if (event.type == SDL_QUIT)
			{
				this->state = STATE_END;
			}

			if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					this->state = STATE_END;
				}
			}

			if (event.type == SDL_JOYDEVICEADDED)
			{

				SDL_Log("INDEX %d\n", event.jdevice.which);

				if (this->indexController < MAX_CONTROLLERS)
				{
					SDL_Log("JOY %d DEVICE ADD\n", this->indexController);

					if (!SDL_IsGameController(event.jdevice.which))
					{
						SDL_Log("  Joystick is not supported by the game controller interface!\n\n");
					}
					else
					{
						this->controller[this->indexController] = SDL_GameControllerOpen(event.jdevice.which);

						if (this->controller[this->indexController])
						{
							char *guid = new char[64];

							SDL_JoystickGetGUIDString(
									SDL_JoystickGetDeviceGUID(event.jdevice.which),
									guid, 64);

							SDL_Log("  Opened Joystick %d\n",
									SDL_GameControllerGetJoystick(this->controller[this->indexController])->ref_count);
							SDL_Log("  Name:              %s\n", SDL_JoystickNameForIndex(event.jdevice.which));
							SDL_Log("  GUID               %s\n", guid);
							SDL_Log("  Number of Axes:    %d\n", SDL_JoystickNumAxes(
										SDL_GameControllerGetJoystick(this->controller[this->indexController])));
							SDL_Log("  Number of Buttons: %d\n", SDL_JoystickNumButtons(
										SDL_GameControllerGetJoystick(this->controller[this->indexController])));
							SDL_Log("  Number of Balls:   %d\n", SDL_JoystickNumBalls(
										SDL_GameControllerGetJoystick(this->controller[this->indexController])));

							this->indexController++;
						}
					}
				}
			}

			if (event.type == SDL_JOYDEVICEREMOVED)
			{
				SDL_Log("JOY DEVICE REMOVED\n");

				SDL_GameController *controller = SDL_GameControllerFromInstanceID(event.jdevice.which);

				SDL_Log("  Name:              %s\n", SDL_GameControllerName(controller));
				SDL_GameControllerClose(controller);

				this->indexController--;
			}

			if (event.type == SDL_CONTROLLERAXISMOTION)
			{
				SDL_Log("CONTROLLER AXIS EVENT\n");
			}

			if (event.type == SDL_CONTROLLERBUTTONDOWN)
			{
				SDL_Log("CONTROLLER BUTTON DOWN EVENT\n");
			}

			if (event.type == SDL_CONTROLLERBUTTONUP)
			{
				SDL_Log("CONTROLLER BUTTON UP EVENT\n");
			}


		}

		for (int i = 0; i < 6; ++i)
		{
			this->world->Step((1.0/this->framerate), 6, 2);
			this->world->ClearForces();
		}

		this->render();

		this->deltaTicks = SDL_GetTicks() - this->startTicks;

		if (this->deltaTicks < (1000 / this->framerate))
		{
			SDL_Delay( (1000 / this->framerate) - this->deltaTicks );
		}

		startTicks = SDL_GetTicks();
	}
}

void Game::destroy(uint8_t withEntity)
{
	if (withEntity)
	{
		for (Entity* obj : this->objects)
		{
			free(obj);
			obj = NULL;
		}
	}

	free(this->world);
    SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
    SDL_Quit();
}
