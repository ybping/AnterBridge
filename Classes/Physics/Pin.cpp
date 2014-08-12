#include "pin.h"

Pin::Pin(b2Transform transform, bool fixed)
	:transform(transform), fixed(fixed), physicBody(NULL)
{

}

Pin::~Pin()
{
	if(physicBody)
		delete physicBody;
}