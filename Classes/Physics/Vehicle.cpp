#include "Vehicle.h"

Vehicle::Vehicle(b2Transform transform, b2World *m_world)
	:transform(transform)
{	
	b2Vec2 vertices[8];
	vertices[0].Set(-1.5f, -0.5f);
	vertices[1].Set(1.5f, -0.5f);
	vertices[2].Set(1.5f, 0.0f);
	vertices[3].Set(0.0f, 0.9f);
	vertices[4].Set(-1.15f, 0.9f);
	vertices[5].Set(-1.5f, 0.2f);
	chassis.Set(vertices, 6);

	b2CircleShape circle;
	circle.m_radius = 0.4f;

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position.Set(0.0f + transform.p.x, 1.0f + transform.p.y);
	m_car = m_world->CreateBody(&bd);
	m_car->CreateFixture(&chassis, 1.0f);

	b2FixtureDef fd;
	fd.shape = &circle;
	fd.density = CAR_DENSITY;
	fd.friction = 0.1f;
	fd.filter.categoryBits = CategoryMastBits::Category_Car;
	fd.filter.maskBits     = CategoryMastBits::Category_Slab | CategoryMastBits::Category_Car; 

	bd.position.Set(-1.0f + transform.p.x, -0.35f + transform.p.y);
	m_wheel1 = m_world->CreateBody(&bd);
	m_wheel1->CreateFixture(&fd);

	bd.position.Set(1.0f + transform.p.x, -0.4f + transform.p.y);
	m_wheel2 = m_world->CreateBody(&bd);
	m_wheel2->CreateFixture(&fd);

	b2WheelJointDef jd;
	b2Vec2 axis(0.0f, 1.0f);

	jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
	jd.motorSpeed = CAR_SPEED;
	jd.maxMotorTorque = 20.0f;
	jd.enableMotor = true;
	jd.frequencyHz = CAR_FREQ;
	jd.dampingRatio = CAR_DAMP;
	m_spring1 = (b2WheelJoint*)m_world->CreateJoint(&jd);

	jd.Initialize(m_car, m_wheel2, m_wheel2->GetPosition(), axis);
	jd.motorSpeed = 0.0f;
	jd.maxMotorTorque = 10.0f;
	jd.enableMotor = false;
	jd.frequencyHz = CAR_FREQ;
	jd.dampingRatio = CAR_DAMP;
	m_spring2 = (b2WheelJoint*)m_world->CreateJoint(&jd);
	//initVehicleBody(m_world);
}

void Vehicle::initVehicleBody(b2World *m_world)
{
	// Vehicle Body
	{
		
		b2Vec2 vertices[8];
		int scale = 1;
		vertices[0].Set(-1.5f, -0.5f);
		vertices[1].Set(1.5f, -0.5f);
		vertices[2].Set(1.5f, 0.0f);
		vertices[3].Set(0.0f, 0.9f);
		vertices[4].Set(-1.15f, 0.9f);
		vertices[5].Set(-1.5f, 0.2f);
		for (int i = 0; i < 8; i++)
		{
			vertices[i] *= scale;
		}
		chassis.Set(vertices, 6);

		

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f * scale + transform.p.x, 1.0f * scale + transform.p.y);
		m_car = m_world->CreateBody(&bd);
		m_car->CreateFixture(&chassis, 1.0f);


		b2CircleShape circle;
		circle.m_radius = 0.8f * scale;

		b2FixtureDef fd;
		fd.shape = &circle;
		fd.density = CAR_DENSITY;
		fd.friction = 0.2f;
		fd.filter.categoryBits = CategoryMastBits::Category_Car;
		fd.filter.maskBits     = CategoryMastBits::Category_Slab | CategoryMastBits::Category_Car; 

		bd.position.Set(-1.0f * scale+transform.p.x, 0.35f * scale +transform.p.y);
		m_wheel1 = m_world->CreateBody(&bd);
		m_wheel1->CreateFixture(&fd);

		bd.position.Set(1.0f * scale +transform.p.x, 0.4f * scale +transform.p.y);
		m_wheel2 = m_world->CreateBody(&bd);
		m_wheel2->CreateFixture(&fd);

		b2WheelJointDef jd;
		b2Vec2 axis(0.0f, 1.0f);

		jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
		jd.motorSpeed = CAR_SPEED;
		jd.maxMotorTorque = 20.0f;
		jd.enableMotor = true;
		jd.frequencyHz = CAR_FREQ;
		jd.dampingRatio = CAR_DAMP;
		m_spring1 = (b2WheelJoint*)m_world->CreateJoint(&jd);

		jd.Initialize(m_car, m_wheel2, m_wheel2->GetPosition(), axis);
		jd.motorSpeed = CAR_SPEED;
		jd.maxMotorTorque = 20.0f;
		jd.enableMotor = true;
		jd.frequencyHz = CAR_FREQ;
		jd.dampingRatio = CAR_FREQ;
		m_spring2 = (b2WheelJoint*)m_world->CreateJoint(&jd);
	}
}

b2Body* Vehicle::getWheelBody(char direction)
{
	if(direction == 'L')
		return m_wheel1;
	if(direction == 'R')
		return m_wheel2;	
	return NULL;
}