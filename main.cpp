#include <core/game.h>
#include "ground.h"
#include "tank.h"


int main(void)
{
	Game::engine.init(1280, 720, 60);

	Ground *ground = new Ground(Game::engine.world);

	Tank   *player1   = new Tank  (Game::engine.world);
	Tank   *player2   = new Tank  (Game::engine.world);

	player1->setColor(0xFF0000FF);
	player2->setColor(0x0000FFFF);


	Game::engine.objects.push_back(ground);
	Game::engine.objects.push_back(player1);
	Game::engine.objects.push_back(player2);


	Game::engine.loop();

	// flas 0 para mantener las entidades en memoria
	// flag 1 para eliminarlas de la memoria
	Game::engine.destroy(1);

	return 0;
}
