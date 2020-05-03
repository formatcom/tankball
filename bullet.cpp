#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <Box2D/Box2D.h>
#include <core/game.h>
#include <core/entity.h>
#include "bullet.h"


class _Bullet : public Entity
{
public:
	_Bullet(b2World * world);
	void update();
	void render(SDL_Renderer *renderer);

private:
	int16_t radius;
	uint8_t step;
};

_Bullet::_Bullet(b2World * world) : Entity(world)
{
	this->active = false;
	this->type   = 2;     // bullet

	this->color.rgba = 0x0FFFF0FF;
	this->radius     = 10;

	this->def->bullet          = true;
	this->def->type            = b2_dynamicBody;
	this->def->linearDamping   = 0.03;
	this->def->angularDamping  = 0.05;

	// fix this
	this->def->position.Set(-1000, 0);

	this->body = this->world->CreateBody(this->def);

	this->body->SetUserData(this);

	b2CircleShape * shape = new b2CircleShape;

	shape->m_radius = this->radius / PTM;

	this->fixture->shape   = shape;
	this->fixture->density = 1.0;

	this->body->CreateFixture(this->fixture);

}

void _Bullet::update()
{

	if (this->active)
	{
		this->step++;

		if (!this->body->IsAwake() || this->step > 290)
		{
			this->active = false;
		}
	}
	else
	{
		this->step = 0;
		this->body->SetTransform(b2Vec2(-1000, 0), 0);
	}
}

void _Bullet::render(SDL_Renderer *renderer)
{
	if (!this->active) return;

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


Bullet::Bullet(b2World * world)
{
	this->len = 3;
	for (uint8_t i = 0; i < this->len; ++i)
	{
		this->entity[i] = new _Bullet(world);
	}
}

void Bullet::shot(b2Vec2 position, float angle, uint8_t power)
{
	for (uint8_t i = 0; i < this->len; ++i)
	{
		_Bullet *bullet = ((_Bullet*)(this->entity[i]));

		bullet->active = true;

		b2Vec2 velocity;
		velocity.x = rand() % 10;
		velocity.y = rand() % 10;

		bullet->getBody()->SetAngularVelocity((rand() % 11) - 5);
		bullet->getBody()->SetLinearVelocity(velocity);
		bullet->getBody()->SetTransform(position, angle);

		b2Vec2 position;
		position.x = 10 * cos(angle);
		position.y = 10 * sin(angle);

		bullet->getBody()->ApplyLinearImpulse(
							b2Vec2(position.x*power*1.6, position.y*power*1.6),
							bullet->getBody()->GetWorldCenter(), true);
	}
}
