#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <Box2D/Box2D.h>
#include "game.h"

Game Game::engine;

int Game::initSDL()
{
	this->window   = NULL;
	this->renderer = NULL;

	if (SDL_Init(
			SDL_INIT_TIMER | SDL_INIT_VIDEO |
			SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC ) != 0)
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

void Game::openGameController(int device)
{
	printf("DEVICE %d\n", device);
	/*
	int maxJoysticks = SDL_NumJoysticks();
	int index = 0;

	for (int i=0; i < maxJoysticks; ++i)
	{
		printf("JOY %d\n", i);
		if (!SDL_IsGameController(i)) continue;
		if (index >= MAX_CONTROLLERS) break;
		printf("CONTROLLER %d\n", i);

		this->controller[index] = SDL_GameControllerOpen(i);
		this->haptic[index]     = SDL_HapticOpen(i);

		if (this->haptic[index] && SDL_HapticRumbleInit(this->haptic[index]) != 0)
		{
			SDL_HapticClose(this->haptic[index]);
			this->haptic[index] = 0;
		}

		index++;
	}
	*/
}

void Game::closeGameController(int device)
{
	for (int i=0; i < MAX_CONTROLLERS; ++i)
	{
		if (this->controller[i])
		{
			if (this->haptic[i])
			{
				SDL_HapticClose(this->haptic[i]);
			}

			SDL_GameControllerClose(this->controller[i]);
		}
	}
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
				printf("KEY %d\n", event.key.keysym.sym);
			}

			if (event.type == SDL_JOYDEVICEADDED)
			{
				printf("JOY DEVICE ADD\n");
				SDL_JoystickOpen(0);
				SDL_GameController *controller = SDL_GameControllerOpen(0);
				if (!controller)
				{
					fprintf(stderr, "Could not open gamecontroller 0: %s\n", SDL_GetError());
				}
			}

			if (event.type == SDL_JOYDEVICEREMOVED)
			{
				printf("JOY DEVICE REMOVED\n");
				SDL_GameControllerClose(0);
				SDL_JoystickClose(0);
			}

			if (event.type == SDL_CONTROLLERDEVICEREMAPPED)
			{
				printf("CONTROLLER DEVICE REMAPPED\n");
			}

			if (event.type == SDL_CONTROLLERAXISMOTION)
			{
				printf("CONTROLLER AXIS EVENT\n");
			}

			if (event.type == SDL_CONTROLLERBUTTONDOWN)
			{
				printf("CONTROLLER BUTTON DOWN EVENT\n");
			}

			if (event.type == SDL_JOYBUTTONDOWN)
			{
				printf("JOY BUTTON DOWN EVENT\n");
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
