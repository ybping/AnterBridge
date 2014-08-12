#ifndef __PIN_H_
#define __PIN_H_

#include "Box2D\Box2D.h"

/* A "Pin" is a join between two bridge segments, usually drawn as a dot in bridge building games.
   It uses joints to connect multiple segments together, and can be "Fixed" aka static. */
class Pin
{

private:

	b2Transform		 transform;    /* The position of this pin while at start. */

	bool			 fixed;        /* Set this to true if the Pin must not move, e.g., attached to ground. */

	void			 *physicBody;   /* Points to the object instance within the Physics world instance. */

public:

	Pin(b2Transform transform, bool fixed = false);

	~Pin();

	bool isFixed(){ return fixed;}
	
	void setFixed(bool fixed){ this -> fixed = fixed;}

	void setPhysicBody(void *physicBody){ this -> physicBody = physicBody;}

	void *getPhysicBody(){ return physicBody;}

	b2Transform getTransform(){ return transform;}

};

#endif
