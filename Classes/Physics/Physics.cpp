#include "Physics.h"

Physics::Physics()
{
	world = NULL;
}

Physics::~Physics()
{
	Destroy();
}

bool Physics::Create()
{
	Destroy();
	/* Do some basic Box2D world setup stuff, setting gravity etc. */
	b2Vec2 gravity;
	gravity.Set(0.0f, -10.0f);
	world = new b2World(gravity);
	world->SetWarmStarting(true);
	world->SetContinuousPhysics(true);
	world->SetSubStepping(false);

	return true;
}

bool Physics::Destroy()
{
	delete world;
	world = NULL;

	return false;
}

void Physics::Step(float TimeStep)
{
	if (world == NULL)
		return;

	world->Step(TimeStep, Simulation_VelocityIterations, Simulation_PositionIterations);
}

void Physics::RemoveJoint(void *Joint)
{
	world->DestroyJoint((b2Joint*)Joint);
}

/* This takes a body that was returned from an AddPin or AddSlab call and sets up a
	transform instance with the current position and angle of the body. */
void Physics::GetTransform(void *Body,b2Transform &transfom)
{
	if(Body == NULL || world == NULL)
		return ;
	b2Body *body = (b2Body*)Body;
	transfom = body->GetTransform();
}

/* This takes a joint that was returned from an AddSupport call and calculates the force
	it is currently experiencing, destroying the joint if it exceeds the specified maximum.
	The calculated force is stored in Result so that the colour of the joint can be calculated. */
void Physics::HandleSupportForce(void *Joint, float Delta, float &Result, float Maximum)
{
	/* If the simulation isn't running, return having not modified a thing. */
	if (Joint == NULL || world == NULL)
		return;

	b2Joint *joint = (b2Joint*)Joint;
	Result = joint->GetReactionForce(Delta).Length();
	if (Result >= Maximum)
	{
		Result = Maximum;
		world->DestroyJoint(joint);
		Joint = NULL;
	}
}

/* This takes a joint that was returned from an AddPin call and calculates the force
	it is currently experiencing, destroying the joint if it exceeds the specified maximum.
	(we separate this and the HandleSupportForce calls so that we can treat Pins and Supports differently). */
void Physics::HandlePinForce(void *Body, float Delta, float Maximum)
{
	if (Body == NULL || world == NULL)
		return;

	Maximum *= Maximum; /* Square the maximum to save us having to compare sqrt's against it. */

	b2Body      *body  = (b2Body*)Body;
	b2JointEdge *edges = body->GetJointList();
	while (edges)
	{
		b2JointEdge *next = edges->next;
		if (edges->joint->GetReactionForce(Delta).LengthSquared() > Maximum)
			world->DestroyJoint(edges->joint);

		edges = next;
	}
}

/* This creates a circular body a the specified location and returns it. */
void* Physics::AddPin(float X, float Y, bool Fixed)
{
	b2Body *result = NULL;

	b2CircleShape shape;
	shape.m_radius              = 0.5f;

	b2FixtureDef  fixture;
	fixture.shape               = &shape;
	fixture.density             = 20.0f;
	fixture.friction            = 0.2f;
	fixture.filter.categoryBits = Category_Pin;
	fixture.filter.maskBits     = 0; /* Pins don't collide with anything. */

	b2BodyDef body;
	body.type = b2_dynamicBody;
	if (Fixed)
		body.type = b2_staticBody;
	body.position.Set(X, Y);
	result = world->CreateBody(&body);
	result->CreateFixture(&fixture);

	return result;
}

/* This takes two Pin->PhysicsBody instances, converts them to native objects (in this case Box2D b2Body objects),
	then constructs a rectangle between them and hooks them up with rotation joints. */
void* Physics::AddStructure(void *Left, void *Right, bool Fixed)
{
	b2Body *result = NULL;
	b2Body *left   = (b2Body*)Left;
	b2Body *right  = (b2Body*)Right;

	/* First get the X,Y positions of the two bodies... */
	b2Vec2  leftPosition  = left->GetPosition();
	b2Vec2  rightPosition = right->GetPosition();
	/* ... then calculate the distance between them, this will be used for the length of the box ... */
	b2Vec2  length        = leftPosition - rightPosition;
	float   slabLength    = length.Length() / 2.0f; /* Box2D specifies a radius of sorts. */
	/* ... calculate the center position of the rectangle, so we can use it as our object origin ... */
	float   centerX       = leftPosition.x + ((rightPosition.x - leftPosition.x) / 2.0f);
	float   centerY       = leftPosition.y + ((rightPosition.y - leftPosition.y) / 2.0f);
	/* ... then work out the angle between the two Pins so we know the orientation of the rectangle. */
	float   angle         = atan2(rightPosition.y - leftPosition.y, rightPosition.x - leftPosition.x);

	/* Create a Box2D shape. */
	b2PolygonShape shape;
	shape.SetAsBox(slabLength, 0.125f);
	/* Create a Box2D fixture set to the above shape, and set the collision index so that it is different to the car. */
	b2FixtureDef fixture;
	fixture.shape               = &shape;
	fixture.density             = 20.0f;
	fixture.friction            = 0.2f;
	fixture.filter.categoryBits = Category_Slab;
	fixture.filter.maskBits     = Category_Car;  /* Slabs do collide with the car. */
	/* Create the Box2D body at the position and angle as calculated above. */
	b2BodyDef body;
	body.position.Set(centerX, centerY);
	body.type = b2_dynamicBody;
	if (Fixed)
		body.type = b2_staticBody;
	body.angle = angle;
	result     = world->CreateBody(&body);
	result->CreateFixture(&fixture);

	/* Connect the slab to the two Pins it is attached to with a revolution joint. */
	b2RevoluteJointDef joint;
	joint.Initialize(left, result, leftPosition);
	world->CreateJoint(&joint);
	joint.Initialize(result, right, rightPosition);
	world->CreateJoint(&joint);

	return result;
}

/* This takes two Pin->PhysicsBody instances, converts them to native objects (in this case Box2D b2Body objects),
	then constructs a distance joint between the two bodies. */
void* Physics::AddSupport(void *Left, void *Right, bool Fixed)
{
	b2Body *left  = (b2Body*)Left;
	b2Body *right = (b2Body*)Right;

	/* Connect the support to the two pins it is attached to with a distance joint */
	b2DistanceJointDef joint;
	joint.Initialize(left, right, left->GetPosition(), right->GetPosition());
	joint.frequencyHz  = JOINT_FREQ;
	joint.dampingRatio = JOINT_DAMP;
	return world->CreateJoint(&joint);
}

/* This simply adds a debug box to our scene. */
void* Physics::AddBox(float X, float Y, float Mass)
{
	if (world == NULL)
		return NULL;

	b2Transform transform(b2Vec2(X, Y), b2Rot(0));
	return new Vehicle(transform, world);
}
