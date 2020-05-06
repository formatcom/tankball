#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/reboot.h>
#include <Box2D/Box2D.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//#include "contact.h"
#include "hack_SDL.h"
#include "game.h"

Game Game::engine;

void Game::findEvent()
{
	DIR *folder;

	struct dirent *dent;

	folder = opendir("/dev/input");
	if (folder)
	{
		while((dent = readdir(folder)))
		{
			if (strncmp(dent->d_name, "event", 5) == 0)
			{
				char device[300];
				char name[256];

				sprintf(device, "/dev/input/%s", dent->d_name);

				this->fdbutton = open(device, O_RDONLY | O_NONBLOCK);
				if (this->fdbutton > 0)
				{
					ioctl(this->fdbutton, EVIOCGNAME(256), name);
					if (strncmp(name, "soc:shutdown_button", 19) != 0)
					{
						close(this->fdbutton); this->fdbutton = -1;
						continue;
					}
					break;
				}
			}
		}
	}
}

int Game::initSDL()
{
	this->window   = nullptr;
	this->renderer = nullptr;

	if (SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER ) != 0)
	{
		return 1;
	}

	if (TTF_Init() == -1)
	{
		return 2;
	}

	if (SDL_CreateWindowAndRenderer(this->width, this->height,
				SDL_WINDOW_SHOWN, &this->window, &this->renderer))
	{
		return 3;
	}

	return 0;
}

void Game::initBox2D()
{

	// no funciona como quiero, luego estudiar bien
	// ContactListener *listener = new ContactListener();

	this->world = new b2World(b2Vec2(0.0, GRAVITY));

	// this->world->SetContactListener(listener);
}

int Game::init(uint16_t width, uint16_t height, uint8_t framerate)
{

	srand(time(NULL));

	this->state     = STATE_INIT;
	this->width     = width;
	this->height    = height;

	this->framerate = framerate;

	this->findEvent(); // only rpi

	// fix this
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

void Game::update()
{
	for (Entity* obj : this->objects)
	{
		obj->update();
	}

}

int8_t Game::slotController()
{
	for (int8_t slot = 0; slot < MAX_CONTROLLERS; ++slot)
	{
		if (this->controller[slot] == nullptr) return slot;
	}

	return -1;
}

int8_t Game::slotController(SDL_JoystickID id)
{
	SDL_GameController *joy = SDL_GameControllerFromInstanceID(id);

	for (int8_t slot = 0; slot < MAX_CONTROLLERS; ++slot)
	{
		if (joy == this->controller[slot])
		{
			return slot;
		}
	}

	return -1;
}

void Game::addController(int deviceIndex, int8_t slot, SDL_GameController *joy)
{
	char *guid = new char[64];

	SDL_JoystickGetGUIDString(
			SDL_JoystickGetDeviceGUID(deviceIndex),
			guid, 64);

	SDL_Log("  Opened Joystick %d\n",
			SDL_GameControllerGetJoystick(joy)->ref_count);
	SDL_Log("  Name:              %s\n", SDL_JoystickNameForIndex(deviceIndex));
	SDL_Log("  GUID               %s\n", guid);
	SDL_Log("  Number of Axes:    %d\n", SDL_JoystickNumAxes(
				SDL_GameControllerGetJoystick(joy)));
	SDL_Log("  Number of Buttons: %d\n", SDL_JoystickNumButtons(
				SDL_GameControllerGetJoystick(joy)));
	SDL_Log("  Number of Balls:   %d\n", SDL_JoystickNumBalls(
				SDL_GameControllerGetJoystick(joy)));

	this->controller[slot] = joy;

}

void Game::removeController(int8_t slot)
{
	char *guid = new char[64];

	SDL_JoystickGetGUIDString(
			SDL_JoystickGetGUID(SDL_GameControllerGetJoystick(
					this->controller[slot])),
			guid, 64);

	SDL_Log("  Name:              %s\n", SDL_GameControllerName(this->controller[slot]));
	SDL_Log("  GUID               %s\n", guid);

	this->controller[slot] = nullptr;
}

void Game::updateAngle(int8_t slot, uint8_t angle)
{
	this->angle[slot] = angle;
}

void Game::updateButton(int8_t slot, uint8_t button, bool down)
{
	// fix this
	if (down)
	{
		if (button == SDL_CONTROLLER_BUTTON_A)
		{
			this->buttons[slot] |= BUTTON_A;

		}else

		if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
		{
			this->buttons[slot] |= BUTTON_LEFT;

		}else

		if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
		{
			this->buttons[slot] |= BUTTON_RIGHT;
		}

	}
	else
	{
		if (button == SDL_CONTROLLER_BUTTON_A)
		{
			this->buttons[slot] &= ~BUTTON_A     & 0xFF;

		}else

		if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
		{
			this->buttons[slot] &= ~BUTTON_LEFT  & 0xFF;

		}else

		if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
		{
			this->buttons[slot] &= ~BUTTON_RIGHT & 0xFF;
		}

	}

}

void Game::loop()
{
	SDL_Event event;
	struct input_event ie;

	while((this->state & STATE_END) == 0)
	{

		// power off only rpi
		if (this->fdbutton > 0)
		{
			while(read(this->fdbutton, &ie, sizeof(struct input_event)) > 0)
			{
				if (ie.type == EV_KEY && ie.code == KEY_POWER)
				{
					sync();
					reboot(RB_POWER_OFF);
				}
			}
		}


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
				int8_t slot = this->slotController();
				SDL_Log("JOY %d DEVICE ADD\n", slot);

				if (slot != -1)
				{
					if (!SDL_IsGameController(event.jdevice.which))
					{
						SDL_Log("  Joystick is not supported by the game controller interface!\n\n");
					}
					else
					{
						SDL_GameController *joy = SDL_GameControllerOpen(event.jdevice.which);

						if (joy) this->addController(event.jdevice.which, slot, joy);
					}
				}
			}

			if (event.type == SDL_JOYDEVICEREMOVED)
			{
				int8_t slot = this->slotController(event.jdevice.which);
				SDL_Log("JOY %d DEVICE REMOVED\n", slot);

				this->removeController(slot);

				SDL_GameController *joy = SDL_GameControllerFromInstanceID(event.jdevice.which);
				SDL_GameControllerClose(joy);

			}

			if (event.type == SDL_CONTROLLERAXISMOTION)
			{
				// SDL_Log("CONTROLLER AXIS EVENT\n");
				if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
				{
					int8_t slot = this->slotController(event.jdevice.which);
					this->updateAngle(slot, GET_ANGLE(event.caxis.value));
				}
			}

			if (event.type == SDL_CONTROLLERBUTTONDOWN)
			{
				// SDL_Log("CONTROLLER BUTTON DOWN EVENT\n");

				int8_t slot = this->slotController(event.jdevice.which);

				this->updateButton(slot, event.cbutton.button, true);
			}

			if (event.type == SDL_CONTROLLERBUTTONUP)
			{
				// SDL_Log("CONTROLLER BUTTON UP EVENT\n");

				int8_t slot = this->slotController(event.jdevice.which);

				this->updateButton(slot, event.cbutton.button, false);
			}


		}

		this->update();

		for (int i = 0; i < 12; ++i)
		{
			this->world->Step((1.0/60), 6, 4);

			this->world->ClearForces();

			for (Entity* entityA : this->objects)
			{

				if (!entityA->getType()) continue;

				for (Entity* entityB : this->objects)
				{
					entityA->contact(entityA, entityB);
				}

			}
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
			obj = nullptr;
		}
	}

	if (this->fdbutton > 0) close(this->fdbutton);

	free(this->world);
    SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
	TTF_Quit();
    SDL_Quit();
}
