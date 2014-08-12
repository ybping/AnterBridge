#include "Bridge.h"


Bridge::Bridge()
{
	vSlabs.clear();
	vPins.clear();
	startPin = NULL;
}

Bridge::~Bridge()
{
	Destroy();
}

bool Bridge::Create(CCPoint left, CCPoint right)
{
	/* destory any existing bridge */
	Destroy();

	/* The left and right most extreme pins are "attached" to our imaginary ground. */
	addPin(left.x, left.y)->setFixed(true); 
	addPin(left.x - 100, left.y)->setFixed(true); 
	
	addPin(right.x, right.y)->setFixed(true);	
	addPin(right.x + 100, right.y)->setFixed(true);

	addSlab(left.x - 100, left.y, left.x, left.y, Slab_Purpose_Structure);
	addSlab(right.x, right.y, right.x + 100, right.y,Slab_Purpose_Structure);

	carPostion = CCPoint(left.x - 50, left.y + 10);

	return true;
}

bool Bridge::Destroy()
{
	/* Destroy the slabs. */
	for(auto item :vSlabs)
		delete item;
	vSlabs.clear();

	/* Destroy the pins */
	for(auto item :vPins)
		delete item;
	vPins.clear();

	editMode     = Bridge_EditMode_Structure;
	running      = false;

	return false;
}

void Bridge::RevokeSlab()
{
	if(vSlabs.size() > 1)
	{
		Slab *item = vSlabs.back();
		vSlabs.pop_back();
		delete item;
	}
}

Pin* Bridge::getPinAt(float X, float Y, float Accuracy)
{
	for(auto pin : vPins)
	{
		if ( (fabs(pin->getTransform().p.x - X) <= Accuracy) && (fabs(pin->getTransform().p.y - Y) <= Accuracy) )
			return pin;
	}
	return NULL;
}

/* This first tries to find a Pin at the current location, returning it if one was found, otherwise it tries to add one. */
Pin* Bridge::addPin(float X, float Y)
{
	Pin *pin = getPinAt(X, Y, 0.5f);
	if (pin != NULL)
		return pin;
	b2Vec2 pose(X, Y);
	pin = new Pin(b2Transform(pose, b2Rot(0)));
	if (pin == NULL)
		return pin;

	vPins.push_back(pin);
	return pin;
}

/* This adds two Pins, one at X1,Y1 and another at X2,Y2.
	These Pins serve to hold up the newly created Slab instance this function will create. */
Slab* Bridge::addSlab(float X1, float Y1, float X2, float Y2, Slab_Purpose Purpose)
{
	Pin  *left  = addPin(X1, Y1);
	Pin  *right = addPin(X2, Y2);
	Slab *slab  = NULL;

	if (left == NULL || right == NULL)
		return slab;
		
	switch (Purpose)
	{
		case Slab_Purpose_Support: 
			slab = new SlabSupport(left, right);
			break;
		case Slab_Purpose_Structure:
			slab = new SlabStructure(left, right); 
			break;
		default:
			break;
	}

	if (slab == NULL)
		return slab;

	vSlabs.push_back(slab);
	return slab;
}

void Bridge::AddCar()
{
	HandleTouch(carPostion.x, carPostion.y);
}

/* This function converts the pin and slab meta-data structures into physical objects within
	the physics engine. */
void Bridge::createSimulation()
{
	/* Clear any existing physics world (genocide, yay!) */
	physics.Create();

	/* First we create bodies for the pins, so that slabs have something to attach to. */
	for(auto pin : vPins)
	{
		/* Each pin gets assigned a pointer to a physics engine body for things to latch onto. */
		void *body = physics.AddPin(pin->getTransform().p.x, pin->getTransform().p.y, pin->isFixed());
		pin->setPhysicBody(body);
	}

	/* Now we create our different types of slabs, attaching each one to two existing pins. */
	for(auto slab : vSlabs)
	{
		slab->Recalculate(); /* Recalculate the angle and length of the slab, as its pins may have moved. */
		switch (slab->getSlabPurpose())
		{
			case Slab_Purpose_Structure:
				slab->setPhysicBody(physics.AddStructure(slab->getPin('L')->getPhysicBody(), slab->getPin('R')->getPhysicBody(), slab->isFixed()));
				break;
			case Slab_Purpose_Support:
				slab->setPhysicBody( physics.AddSupport(slab->getPin('L')->getPhysicBody(), slab->getPin('R')->getPhysicBody(), slab->isFixed()));
				break;
			default:
				break;
		}
	}
	running = true;
}

void Bridge::GetBoxVertices(float X, float Y, float Width, float Height, float Angle, CCPoint *Vertices)
{
	float cosine       = cos(Angle);
	float sine         = sin(Angle);
	float widthCosine  = (Width / 2.0f) * cosine;
	float heightCosine = (Height / 2.0f) * cosine;
	float widthSine    = (Width / 2.0f) * sine;
	float heightSine   = (Height / 2.0f) * sine;
	float urX          = X - widthCosine  - heightSine;
	float urY          = Y + heightCosine - widthSine;
	float blX          = X + widthCosine  + heightSine;
	float blY          = Y - heightCosine + widthSine;
	Vertices[0] = CCPoint(blX,blY);
	Vertices[1] = CCPoint(urX,urY);
		

}

/* This is called for every single "step" in the game.
	It is responsible for advancing the physics engine and drawing the bits of our level */
void Bridge::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	float timeStep = 1 / 60.0;
	physics.Step(timeStep);
		
	for(auto slab : vSlabs)
	{
		switch (slab->getSlabPurpose())
		{
			case Slab_Purpose_Structure: /* Draw these as boxes around the X,Y co-ords of the physics entity. */
			{
				b2Transform transform = slab -> getTransform();
				physics.GetTransform(slab->getPhysicBody(),transform);
				float         angle  = transform.q.GetAngle();
				float         x      = cos(angle) * (slab->getLength() / 2.0f);
				float         y      = sin(angle) * (slab->getLength() / 2.0f);

				CCPoint vertices[2];
				GetBoxVertices(transform.p.x, transform.p.y, slab->getLength(), 0.5f, transform.q.GetAngle(),vertices);
				glLineWidth( 12.0f );  
				ccDrawColor4B(0,128,128,255);  
				ccDrawLine(vertices[0]*SCALE, vertices[1]*SCALE); 

				break;
			}
			case Slab_Purpose_Support: /* Draw these simply as lines between the two pins. */
			{
				float force = 0.0f;
				/* Try get the force exerted on the joint, knowing that if it's too much, the joint will be deleted
					and the slab->PhysicBody will be set to NULL, meaning we don't need to draw it. */
				physics.HandleSupportForce(slab->getPhysicBody(), timeStep, force, MAX_FORCE_JOINT_BREAK);
				force = force / (float)MAX_FORCE_JOINT_BREAK;

				if (running == false || slab->getPhysicBody() != NULL)
				{
					unsigned char red    = 255;                                      /* We want the joints to show red when they are stressed, so this stays constant. */
					unsigned char green  = (unsigned char)(255 - (force * 255.0f));  /* Green and blue must decrease with an increase in Force, so that the colour */
					unsigned char blue   = green;                                    /* gets a red tint to it. */
						
					glLineWidth( 12.0f );  
					ccDrawColor4B(red,green,blue,255);  
					Pin *leftPin = slab->getPin('L');
					b2Transform leftTrans = leftPin -> getTransform();
					physics.GetTransform(leftPin->getPhysicBody(),leftTrans);

					Pin *rightPin = slab->getPin('R');
					b2Transform rightTrans = rightPin -> getTransform();
					physics.GetTransform(rightPin->getPhysicBody(),rightTrans);

					ccDrawLine(CCPoint(leftTrans.p.x, leftTrans.p.y)*SCALE, CCPoint(rightTrans.p.x, rightTrans.p.y)*SCALE); 
				}
				break;
			}
		}
	}

	glLineWidth(2);
	ccDrawColor4B(0, 255, 255, 255);
	/* Optionally draw all our pins. */
	for(auto pin : vPins)
	{
		physics.HandlePinForce(pin->getPhysicBody(), timeStep, MAX_FORCE_JOINT_BREAK);
		b2Transform transform = pin -> getTransform();
		physics.GetTransform(pin->getPhysicBody(), transform);
		ccDrawCircle(CCPoint(transform.p.x, transform.p.y) * SCALE, 10 * SCALE, CC_DEGREES_TO_RADIANS(0), 50, false);
	}

	if (running)
	{
		for (int box = 0; box < boxCount; box++)
		{
			b2Transform transform;
			Vehicle *car = (Vehicle *) boxes[box];
			physics.GetTransform(car -> getCarBody(), transform);
			
			// draw car body chassis shap
		
			Vec2 vertices[6];
			vertices[0] = Vec2(-1.5f, -0.5f);
			vertices[1] = Vec2(1.5f, -0.5f);
			vertices[2] = Vec2(1.5f, 0.0f);
			vertices[3] = Vec2(0.0f, 0.9f);
			vertices[4] = Vec2(-1.15f, 0.9f);
			vertices[5] = Vec2(-1.5f, 0.2f);
			float scale = 20;
			for (int i = 0; i < 6; i++)
			{
				vertices[i] *= scale;
				vertices[i].x += transform.p.x;
				vertices[i].y += transform.p.y;
			}
			DrawPrimitives::drawSolidPoly(vertices, 6, Color4F(0.5f, 0.5f, 1, 1 ) );
			
			// draw left wheel shap
			physics.GetTransform(car -> getWheelBody('L'), transform);
			DrawPrimitives::drawCircle(Vec2(-1.0f, -0.35) * scale + Vec2(transform.p.x, transform.p.y) , 8, transform.q.GetAngle(), 50, false);

			physics.GetTransform(car -> getWheelBody('R'), transform);
			DrawPrimitives::drawCircle(Vec2(1.0f, -0.4) * scale + Vec2(transform.p.x, transform.p.y), 8, transform.q.GetAngle(), 50, false);
		}
	
	}
		
}

/* Stop the simulation, but keep the bridge in tact. */
void Bridge::Stop()
{
	physics.Destroy();
	memset(boxes, 0, sizeof(void*) * MAX_BLOCKS);
	boxCount = 0;
	running = false;
}

/* Swap to simulation mode. */
void Bridge::Start()
{
	Stop();
	createSimulation();
}

void Bridge::SetEditMode(Bridge_EditMode EditMode)
{
	editMode = EditMode;
}

/* This function is incredibly simple for now, accepting only co-ordinates of incoming touches/mouse clicks.
	It will be expanded later to include an ID and time of the touch so that multi-touch can be dealt with. */
void Bridge::HandleTouch(float X, float Y)
{
	switch (editMode)
	{
		case Bridge_EditMode_Car:
		{
			if (running && boxCount < MAX_BLOCKS)
			{
				boxes[boxCount] = physics.AddBox(X, Y, 10.0f);
				boxCount++;
			}
			break;
		}
		case Bridge_EditMode_Structure:
		case Bridge_EditMode_Support:
		{
			if (startPin == NULL)
			{
				startPin = addPin(X, Y);
			}
			else
			{
				Pin *pin = addPin(X, Y);
				if (pin != startPin)
				{
					addSlab(startPin->getTransform().p.x, startPin->getTransform().p.y, pin->getTransform().p.x, pin->getTransform().p.y, editMode == Bridge_EditMode_Structure ? Slab_Purpose_Structure : Slab_Purpose_Support);
					startPin = NULL;
				}
			}
			break;
		}
	}
}

/* This simply creates a test bridge (saving the user/developer from having to click out a bridge every time).
	This is where our "load" function will come in future. */
void Bridge::CreateTestBridge(CCPoint leftPoint, CCPoint rightPoint, float width)
{
	Create(leftPoint, rightPoint);

	/* We create several slabs in two parallel straight lines. The lower level is the "structure"/"road" level,
		the upper level are the support structures, offset horizontally exactly half the width of a slab.
		These two lines are linked by more support structures, so that we have a nice half-crosshatch pattern. */
	int index;
	int slabCount = (rightPoint.x - leftPoint.x) / width;
	double left = leftPoint.x, right;
	for (index = 0; index < slabCount; index++)
	{
		right = left + width;

		float middle = left + (right - left) / 2.0f;

		addSlab(left,   leftPoint.y, right,  rightPoint.y, Slab_Purpose_Structure);
		addSlab(left,   leftPoint.y, middle, width, Slab_Purpose_Support);
		addSlab(middle, width, right,  rightPoint.y, Slab_Purpose_Support);
		if (index > 0)
			addSlab(middle, width, middle - width, width, Slab_Purpose_Support);

		left += width;
	}
}
