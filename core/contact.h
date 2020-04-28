#ifndef _CORE_CONTACT_H_
#define _CORE_CONTACT_H_

#include <Box2D/Box2D.h>

class ContactListener : public b2ContactListener {

public:
    ContactListener() : b2ContactListener() {};
    ~ContactListener() {};

    virtual void BeginContact(b2Contact* contact);

    virtual void EndContact(b2Contact* contact);

};

#endif
