#ifndef __BRIDGE_H_
#define __BRIDGE_H_

#include "physics.h"
#include "pin.h"
#include "slab.h"
#include "cocos2d.h"
using namespace cocos2d;

#include <vector>
using namespace std;

/* This class mostly holds editing/meta data for bridge creation.
   For now it is very simple, holding only a list of pins and slabs that make up the bridge.
   This class is/will be used for adding/removing/moving items around in the bridge, handling input, and finally
   converting the user data into physics engine data and running the simulation thereof. */

class Bridge : public cocos2d::CCNode
{
private:
	/* A list of slabs that make up the bridge. */
	vector<Slab *> vSlabs;

	/* A list of pins/joins that make up the bridge. */
	vector<Pin *> vPins;
	
	Pin *startPin;

	/* The abstracted physics engine, be it Box2D, Chipmunk, etc. */
	Physics          physics; 

	Bridge_EditMode  editMode;

	bool             running;
	
	/* The following is temporary stuff to test the bridge with for now, will be replaced by cars. */
	void            *boxes[MAX_BLOCKS];

	int              boxCount;

	CCPoint			carPostion;

private:

	/* This returns a Pin instance at the specified co-ordinates with the specified accuracy, or NULL if none are found.
	   Accuracy caters for touches on screens, where the finger normally touches "more or less" around an area. */
	Pin* getPinAt(float X, float Y, float Accuracy);

	/* This first tries to find a Pin at the current location, returning it if one was found, otherwise it tries to add one. */
	Pin* addPin(float X, float Y);
	

	/* This adds two Pins, one at X1,Y1 and another at X2,Y2.
	   These Pins serve to hold up the newly created Slab instance this function will create. */
	Slab* addSlab(float X1, float Y1, float X2, float Y2, Slab_Purpose Purpose);

	/* This function converts the pin and slab meta-data structures into physical objects within
	   the physics engine. */
	void createSimulation();

public:
	Bridge();

	~Bridge();

	bool Create(CCPoint left, CCPoint right);

	bool Destroy();

	void GetBoxVertices(float X, float Y, float Width, float Height, float Angle, CCPoint *Vertices);
	/* This is called for every single "step" in the game.
	   It is responsible for advancing the physics engine and drawing the bits of our level */
	void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);

	/* Stop the simulation, but keep the bridge in tact. */
	void Stop();

	/* Swap to simulation mode. */
	void Start();

	void SetEditMode(Bridge_EditMode EditMode);

	/* This function is incredibly simple for now, accepting only co-ordinates of incoming touches/mouse clicks.
	   It will be expanded later to include an ID and time of the touch so that multi-touch can be dealt with. */
	void HandleTouch(float X, float Y);

	/* Revoke the last added slab */
	void RevokeSlab();

	/* Add test car */
	void AddCar();

	/* This simply creates a test bridge (saving the user/developer from having to click out a bridge every time).
	   This is where our "load" function will come in future. */
	void CreateTestBridge(CCPoint left, CCPoint right, float width);

};

#endif
