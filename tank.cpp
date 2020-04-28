#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <Box2D/Box2D.h>
#include <core/game.h>
#include <core/entity.h>
#include "tank.h"


Tank::Tank(Controller *controller, Bullet *bullet, b2World * world) : Entity(world)
{

	this->life = 9;
	this->type = 1; // Tank

	this->bullet     = bullet;
	this->controller = controller;
	this->color.rgba = 0xFFFFFFFF;
	this->radius     = 16;

	this->def->bullet          = true;
	this->def->type            = b2_dynamicBody;
	this->def->linearDamping   = 0.05;
	this->def->angularDamping  = 0.45;

	this->def->position.Set(rand() % ( Game::engine.getWidth() - this->radius ) + this->radius, 100);

	this->body = this->world->CreateBody(this->def);

	this->body->SetUserData(this);

	b2CircleShape * shape = new b2CircleShape;

	shape->m_radius = this->radius / PTM;

	this->fixture->shape   = shape;
	this->fixture->density = 1.0;

	this->body->CreateFixture(this->fixture);

}

void Tank::contact(Entity *entityA, Entity *entityB, bool end){

	if (entityA == this && entityB->getType() == 2 && entityB->active) // bullet
	{
		this->life--;

		float angle = entityB->getAngle();

		this->body->ApplyLinearImpulse(
				b2Vec2(     (this->radius*cos(angle) *  75) * -1,
					  		this->radius*sin(angle) *  95),
							this->body->GetWorldCenter(), true);


		entityB->active = false;
	}
}

void Tank::update()
{
	uint8_t buttons = Game::engine.getButtons(this->slot);

	uint8_t turn = this->controller->getPlayer();
	if (turn == this->slot && this->controller->state & TANKBALL_STATE_RUNNING)
	{

		// remove notify
		if (buttons && this->controller->state & TANKBALL_STATE_INFO &&
				      ~this->controller->state & TANKBALL_STATE_RECV )
		{
			this->controller->state |= TANKBALL_STATE_SET_MOVE;
			this->controller->state |= TANKBALL_STATE_RECV;
		}

		if (this->controller->state & TANKBALL_STATE_SET_MOVE)
		{
			if (buttons & BUTTON_RIGHT)
			{
				this->body->ApplyLinearImpulse(b2Vec2( 15.0, 0), this->body->GetWorldCenter(), true);
			}

			if (buttons & BUTTON_LEFT)
			{
				this->body->ApplyLinearImpulse(b2Vec2(-15.0, 0), this->body->GetWorldCenter(), true);
			}

			// release BUTTON_A
			if (!(buttons & BUTTON_A) && lastButtons & BUTTON_A)
			{
				this->controller->state &= ~TANKBALL_STATE_SET_MOVE & 0xFF;
				this->controller->state |= TANKBALL_STATE_SET_ANGLE;
			}
		}else

		if (this->controller->state & TANKBALL_STATE_SET_ANGLE)
		{

			b2Vec2 position = this->body->GetPosition();
			float angle = this->body->GetAngle();

			if (buttons & BUTTON_RIGHT)
			{
				this->body->SetTransform(position, angle+=0.1);
			}

			if (buttons & BUTTON_LEFT)
			{
				this->body->SetTransform(position, angle-=0.1);
			}

			if (!(buttons & BUTTON_A) && lastButtons & BUTTON_A)
			{

				this->bullet->shot(
						b2Vec2(
						   	position.x + ((this->radius*2)*cos(angle)),
					  		position.y + ((this->radius*2)*sin(angle)) - this->radius),
							angle, 75);


				// Este deberia cambiar a power de momento esta hardcode
				/*
				this->controller->state &= ~TANKBALL_STATE_SET_ANGLE & 0xFF;
				this->controller->state |= TANKBALL_STATE_SET_MOVE;
				this->controller->next();
				*/
			}
		}
	}

	this->lastButtons = buttons;
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
