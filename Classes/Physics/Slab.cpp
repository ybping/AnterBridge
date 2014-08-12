#include "slab.h"

void Slab::initialise()
{
	if (leftPin != NULL && rightPin != NULL)
	{
		b2Vec2 diffVect = rightPin -> getTransform().p - leftPin -> getTransform().p;
		float angle       = atan2(diffVect.y, diffVect.x);
		length            = diffVect.Length();
		b2Vec2 temp = rightPin -> getTransform().p + leftPin -> getTransform().p;
		transform = b2Transform(b2Vec2(temp.x * 0.5, temp.y * 0.5), b2Rot(angle));
		fixed = leftPin->isFixed() && rightPin -> isFixed();
	}
}

Slab::Slab(Pin *leftPin, Pin *rightPin)
	:leftPin(leftPin), rightPin(rightPin), physicBody(NULL)
{
	purpose = Slab_Purpose_Invalid;
	fixed = false;
	initialise();
}

Pin *Slab::getPin(char direction)
{
	if(direction == 'L')
		return leftPin;
	else if (direction == 'R')
		return rightPin;
}

void Slab::Recalculate()
{
	initialise();
}