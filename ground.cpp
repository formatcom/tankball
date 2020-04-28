#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <Box2D/Box2D.h>
#include <core/game.h>
#include <core/entity.h>
#include "ground.h"


Ground::Ground(Controller *controller, b2World * world) : Entity(world)
{

	this->controller = controller;

	this->color.rgba = 0xFFFFFFFF;

	this->def->position.Set(0, Game::engine.getHeight()-50);

	this->generate();
}

void Ground::generate()
{

	this->body = this->world->CreateBody(this->def);

	this->body->SetUserData(this);

	b2EdgeShape * shape = new b2EdgeShape;

	this->fixture->shape       = shape;
	this->fixture->restitution = 0.40;
	this->fixture->friction    = 0.25;

	// Genera el suelo
	b2Vec2 vertex1 = b2Vec2(0.0, 0.0);

	for (int i = -300; i < Game::engine.getWidth()+300; i+= Game::engine.getWidth()/60)
	{

		uint8_t s = rand() % 31 + 5;

		b2Vec2 vertex2 = b2Vec2(i, s * cos( vertex1.x / ( 12.0 * M_PI )));

		shape->Set(vertex1, vertex2);
		this->body->CreateFixture(this->fixture);

		vertex1 = vertex2;
	}

	delete shape;
}

void Ground::update()
{
	if (this->controller->state & TANKBALL_STATE_RESTART)
	{
		this->world->DestroyBody(this->body);
		this->generate();
	}
}

void Ground::render(SDL_Renderer *renderer)
{
	for (b2Fixture* f = this->body->GetFixtureList(); f; f = f->GetNext())
	{
		b2EdgeShape *s = dynamic_cast<b2EdgeShape*>(f->GetShape());

		b2Vec2 vertex1 = this->body->GetWorldPoint(s->m_vertex1);
		b2Vec2 vertex2 = this->body->GetWorldPoint(s->m_vertex2);

		lineRGBA(renderer, vertex1.x, vertex1.y, vertex2.x, vertex2.y,
				this->color.r, this->color.g, this->color.b, this->color.a);
	}

}
