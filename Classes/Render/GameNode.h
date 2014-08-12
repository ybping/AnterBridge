#ifndef __GAMEMODE_H_
#define __GAMEMODE_H_
 
#include "cocos2d.h"
using namespace cocos2d;

#include "Bridge.h"
#include "../setting.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;


typedef enum Mode
{
	Mode_Building = 0,
	Mode_Testing,
};

class GameNode : public cocos2d::CCNode
{
private:
	Bridge								bridge;
	Mode								mode;
	CCPoint								leftPoint;
	CCPoint								rightPoint;
	bool								isButtonDown;
	Vector<CCNode *>					vPinBtn;
	
	/*	add mouse events */
	void								onButtonTouchDown(CCObject *object, Control::EventType controlEvent);

	/* add menu bar */
	void								AddMenuList();
	void								AddBridgePlank(CCObject *object);		// change bridge edit mode : add a plank
	void								AddBridgeRope(CCObject *object);		// change bridge edit mode : add a rope to support the bridge plank
	void								AddBridgeCar(CCObject *object);			// change bridge edit mode : add a car on the bridge
	void								RevokeSlab(CCObject *object);			// Revoke a bridge slab
	void								TestBridgeStructure(CCObject *object);	// change bridge edit mode : start to test the bridge structure
	void								AddPinItems();
	void								update(float time);
	void								setPinBtnVisiable(bool flag){ for (auto btn : vPinBtn)	btn->setVisible(flag);}
public:
	GameNode();
	
	~GameNode();

	bool Create();
	
	bool Destroy();

	bool Step();
	
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
};

#endif
