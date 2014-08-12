#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include <Box2D/Box2D.h>
#include "../setting.h"

class Vehicle
{
private:
	b2Transform		transform;

	b2Body			* m_car;
	
	b2PolygonShape  chassis;

	b2Body			* m_wheel1;
	b2Body			* m_wheel2;

	b2WheelJoint	* m_spring1;
	b2WheelJoint	* m_spring2;

public:
	Vehicle(b2Transform transform, b2World *m_world);
	
	void initVehicleBody(b2World *m_world);

	b2Body *getCarBody(){return m_car;}

	b2PolygonShape getChassisShape(){ return chassis;}

	b2Body  *getWheelBody(char direction);
};



#endif
