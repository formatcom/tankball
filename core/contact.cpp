#include <Box2D/Box2D.h>
#include "entity.h"
#include "contact.h"


void ContactListener::BeginContact(b2Contact *contact)
{
	Entity *entityA = (Entity*)contact->GetFixtureA()->GetBody()->GetUserData();
	Entity *entityB = (Entity*)contact->GetFixtureB()->GetBody()->GetUserData();

	entityA->contact(entityA, entityB, false);
	entityB->contact(entityB, entityA, false);
}

void ContactListener::EndContact(b2Contact *contact)
{
	Entity *entityA = (Entity*)contact->GetFixtureA()->GetBody()->GetUserData();
	Entity *entityB = (Entity*)contact->GetFixtureB()->GetBody()->GetUserData();

	entityA->contact(entityA, entityB, true);
	entityB->contact(entityB, entityA, true);
}
