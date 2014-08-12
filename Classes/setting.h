#ifndef __SETTING_H__
#define __SETTING_H__

const double SCALE = 1.0;

/* Physical engine solver interations */
typedef enum SimulationArgs
{
	Simulation_VelocityIterations = 8,
	Simulation_PositionIterations = 3,
};

/* The BREAK_AT_FORCE value is another "tweak until it feels right" value. It determines how much force can be
   exerted on a joint before it is considered broken.
*/
const double MAX_FORCE_JOINT_BREAK	= 50;

/*
   The MAX_BLOCKS value is debug information at the moment, simply stating how many boxes you can add to the level.
   This will be removed and replaced with the instances of cars in the level. 
*/
#define MAX_BLOCKS      1

/* Bridge edit mode */
typedef enum Bridge_EditMode
{
	Bridge_EditMode_Support = 0,
	Bridge_EditMode_Structure,
	Bridge_EditMode_Car,
};


/* What follows are the different types of slab possible. Derived classes should set their "Purpose" to the value
   appropriate to their function. */
typedef enum Slab_Purpose
{
	Slab_Purpose_Invalid = 0,
	Slab_Purpose_Support,		 /* A support structure, like those wire cables on bridges. Cars do not collide with these. */
	Slab_Purpose_Structure,    /* A solid piece of e.g., road. Cars collide with these slabs. */
};


/* These are settings that need tweaking to get a good "feeling" bridge.
   The first two work together in deciding how bouncy and stiff the bridge is, which decides what the breaking force
   should be.   I'm a dunce when it comes to maths, so I don't know if there is a "proper" way to work this stuff out,
   I pretty much mess around with values until something good comes out.
*/
#define JOINT_FREQ      15.0f
#define JOINT_DAMP      0.5f

/* These are the fixture.filter.categoryBits for the different types of bodies in the physical bridge scene.
   The car collides with slabs, likewise slabs collide with a car, but pins are basically a non-collidable entity. */
typedef enum CategoryMastBits
{
	Category_Car  = 1 << 1,
	Category_Slab = 1 << 2,
	Category_Pin  = 1 << 3,
};

#define CAR_DENSITY 100
#define CAR_SPEED -1000.0f
#define CAR_FREQ 4
#define CAR_DAMP 0.1f

typedef enum PinMap
{
	rows = 5,
	cols = 5,
};
#endif
