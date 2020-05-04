#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <Box2D/Box2D.h>
#include <core/game.h>
#include <core/entity.h>
#include "limit.h"


Limit::Limit(b2World * world) : Entity(world)
{

	this->active = false;
	this->color.rgba = 0xFFFFFFFF;

	this->def->position.Set(0, 0);

	this->body = this->world->CreateBody(this->def);

	this->body->SetUserData(this);

	b2EdgeShape * shape = new b2EdgeShape;

	this->fixture->shape   = shape;
	this->fixture->restitution = 0.80;

	shape->Set(b2Vec2(0, 0), b2Vec2(Game::engine.getWidth()-1, 0));

	this->body->CreateFixture(this->fixture);

	shape->Set(b2Vec2(Game::engine.getWidth()-1, 0),
			   b2Vec2(Game::engine.getWidth()-1,
			          Game::engine.getHeight()-1));

	this->body->CreateFixture(this->fixture);

	shape->Set(b2Vec2(Game::engine.getWidth()-1,
				      Game::engine.getHeight()-1),
			b2Vec2(0, Game::engine.getHeight()-1));

	this->body->CreateFixture(this->fixture);

	shape->Set(b2Vec2(0, 0), b2Vec2(Game::engine.getWidth()-1, 0));

	this->body->CreateFixture(this->fixture);

	shape->Set(b2Vec2(0, Game::engine.getHeight()-1), b2Vec2(0, 0));

	this->body->CreateFixture(this->fixture);
}

void Limit::render(SDL_Renderer *renderer)
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
