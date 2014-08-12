#ifndef __SLAB_H_
#define __SLAB_H_

#include "pin.h"
#include "../setting.h"

class Slab
{
protected:
	Slab_Purpose		purpose;			/* The purpose/material of this slab in the bridge, gets set by derived class constructors. */
	Pin					*leftPin;			/* The first pin this slab is connected to, for simplicity sake, the "left" pin. */
	Pin					*rightPin;			/* The second pin, referred to as the "right" pin. */
	b2Transform			transform;			/* Describes the init position and rotation of this slab. */
	void				*physicBody;		/* Points to the object instance within the Physics world instance. */
	float				length;				/* The length of the slab, we need this for rendering. */
	bool				fixed;
	void initialise();

public:
	Slab(Pin *leftPin, Pin *rightPin);

	void Recalculate();

	Slab_Purpose getSlabPurpose(){ return purpose; }

	void setPhysicBody(void *physicBody){ this -> physicBody = physicBody;}

	void *getPhysicBody() { return physicBody;}

	Pin *getPin(char direction);

	float getLength(){ return length;}

	b2Transform getTransform(){ return transform;}

	bool isFixed(){ return fixed;}
};


class SlabStructure : public Slab
{

public:
	SlabStructure(Pin *Left, Pin *Right) : Slab(Left, Right)
	{
		purpose = Slab_Purpose_Structure;
	}
};

class SlabSupport : public Slab
{
public:
	SlabSupport(Pin *Left, Pin *Right) : Slab(Left, Right)
	{
		purpose = Slab_Purpose_Support;
	}
};

#endif
