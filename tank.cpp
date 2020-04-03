#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <Box2D/Box2D.h>
#include <core/game.h>
#include <core/entity.h>
#include "tank.h"


Tank::Tank(b2World * world) : Entity(world)
{

	this->color.rgba = 0xFFFFFFFF;
	this->radius     = 16;

	this->def->bullet          = true;
	this->def->type            = b2_dynamicBody;
	this->def->linearDamping   = 0.05;
	this->def->angularDamping  = 0.45;

	this->def->position.Set(rand() % ( Game::engine.getWidth() - this->radius ) + this->radius, 100);

	this->body = this->world->CreateBody(this->def);

	b2CircleShape * shape = new b2CircleShape;

	shape->m_radius = this->radius / PTM;

	this->fixture->shape   = shape;
	this->fixture->density = 1.0;

	this->body->CreateFixture(this->fixture);

}

void Tank::render(SDL_Renderer *renderer)
{
	b2Vec2  position = this->body->GetPosition();
	float32 angle    = this->body->GetAngle();

	circleRGBA(renderer, position.x, position.y-this->radius, this->radius,
						 this->color.r,
						 this->color.g,
						 this->color.b,
						 this->color.a);

	lineRGBA(renderer, position.x, position.y - this->radius,
					   position.x + (this->radius*cos(angle)),
					   position.y + (this->radius*sin(angle)) - this->radius,
					   0, 0xFF, 0, 0xFF);

}
