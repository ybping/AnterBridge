#ifndef __PHYSICS_H_
#define __PHYSICS_H_

#include <Box2D/Box2D.h>
#include "Vehicle.h"
#include "../setting.h"

class Physics
{

protected:
	b2World *world;

public:
	Physics();

	~Physics();

	bool Create();

	bool Destroy();

	void Step(float TimeStep);

	void RemoveJoint(void *Joint);

	/* This takes a body that was returned from an AddPin or AddSlab call and sets up a
	   transform instance with the current position and angle of the body. */
	void GetTransform(void *Body,b2Transform &transfom);

	/* This takes a joint that was returned from an AddSupport call and calculates the force
	   it is currently experiencing, destroying the joint if it exceeds the specified maximum.
	   The calculated force is stored in Result so that the colour of the joint can be calculated. */
	void HandleSupportForce(void *Joint, float Delta, float &Result, float Maximum);

	/* This takes a joint that was returned from an AddPin call and calculates the force
	   it is currently experiencing, destroying the joint if it exceeds the specified maximum.
	   (we separate this and the HandleSupportForce calls so that we can treat Pins and Supports differently). */
	void HandlePinForce(void *Body, float Delta, float Maximum);

	/* This creates a circular body a the specified location and returns it. */
	void* AddPin(float X, float Y, bool Fixed);

	/* This takes two Pin->PhysicsBody instances, converts them to native objects (in this case Box2D b2Body objects),
	   then constructs a rectangle between them and hooks them up with rotation joints. */
	void* AddStructure(void *Left, void *Right, bool Fixed);

	/* This takes two Pin->PhysicsBody instances, converts them to native objects (in this case Box2D b2Body objects),
	   then constructs a distance joint between the two bodies. */
	void* AddSupport(void *Left, void *Right, bool Fixed);

	/* This simply adds a debug box to our scene. */
	void* AddBox(float X, float Y, float Mass);

};

#endif
