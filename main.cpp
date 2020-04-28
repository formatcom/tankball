#include <SDL2/SDL_ttf.h>
#include <core/game.h>
#include "controller.h"
#include "ground.h"
#include "tank.h"
#include "logger.h"
#include "bullet.h"


int main(void)
{

	Controller *controller = new Controller();

	Game::engine.init(1280, 720, 60);

	TTF_Font *font = TTF_OpenFont("joystixmonospace.ttf", 16);

	Game::engine.setFont(font);

	Ground *ground    = new Ground(controller, Game::engine.world);

	Bullet *bullet = new Bullet(Game::engine.world);

	Tank   *player1   = new Tank   (controller, bullet, Game::engine.world);
	Tank   *player2   = new Tank   (controller, bullet, Game::engine.world);

	player1->name = "Player 1 RED ";
	player1->slot = 0;
	player2->name = "Player 2 BLUE";
	player2->slot = 1;

	Logger *logger    = new Logger (controller);

	player1->setColor(0xFF0000FF);
	player2->setColor(0x0000FFFF);

	Game::engine.objects.push_back(ground);
	Game::engine.objects.push_back(player1);
	Game::engine.objects.push_back(player2);
	Game::engine.objects.push_back(logger);
	Game::engine.objects.push_back(controller);

	logger->objects.push_back(player1);
	logger->objects.push_back(player2);

	for (uint8_t i = 0; i < bullet->len; ++i)
	{
		Game::engine.objects.push_back(bullet->entity[i]);
	}

	controller->objects = Game::engine.objects;

	Game::engine.loop();

	// flas 0 para mantener las entidades en memoria
	// flag 1 para eliminarlas de la memoria
	Game::engine.destroy(1);

	return 0;
}
