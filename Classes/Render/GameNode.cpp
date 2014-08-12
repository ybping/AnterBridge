#include "GameNode.h"

GameNode::GameNode()
{
	/* Add menu list items */
	AddMenuList();
	/* Add Pin items */
	AddPinItems();

	Create();
}

GameNode::~GameNode()
{
	Destroy();
}

bool GameNode::Create()
{
	if (!bridge.Create(leftPoint / SCALE, rightPoint  / SCALE)) exit(-1);

	///* fllowing lines test for develper, comment it if necessary */
	CCSize size = CCDirector::getInstance()->getWinSize();
	int length =  size.width / PinMap::cols;
	bridge.CreateTestBridge(leftPoint / SCALE, rightPoint  / SCALE, length);
    bridge.SetEditMode(Bridge_EditMode_Car);
	this->TestBridgeStructure(this); 


	//bridge.SetEditMode(Bridge_EditMode_Structure);
	//mode = Mode_Building;

	
	return true;
}

void GameNode::update(float timeStep){
	bridge.update(timeStep);
}

void GameNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	CCNode::draw(renderer,transform,flags);
	bridge.draw(renderer,transform,flags);
}


bool GameNode::Destroy()
{
	bridge.Destroy();
	return false;
}

void GameNode::onButtonTouchDown(CCObject *object, Control::EventType controlEvent)
{
	if(mode != Mode_Testing)
	{
		ControlButton *button = (ControlButton*)object;
		CCPoint point = button->getPosition() / SCALE;
		bridge.HandleTouch(point.x, point.y);
	}
}

void GameNode::AddMenuList()
{
	CCSize size = CCDirector::getInstance()->getWinSize();

	//实现图标的竖排排列  
    CCMenuItemImage *menuImage1=CCMenuItemImage::create("addPlank.gif", "add.gif",this,menu_selector(GameNode::AddBridgePlank));  
	CCMenuItemImage *menuImage2=CCMenuItemImage::create("addRope.gif", "add.gif",this,menu_selector(GameNode::AddBridgeRope));
	CCMenuItemImage *menuImage3=CCMenuItemImage::create("back.gif", "back.png",this,menu_selector(GameNode::RevokeSlab));  
	CCMenuItemImage *menuImage4=CCMenuItemImage::create("car.png", "add.gif",this,menu_selector(GameNode::AddBridgeCar));  
    CCMenuItemImage *menuImage5=CCMenuItemImage::create("accept.png", "accept.png",this,menu_selector(GameNode::TestBridgeStructure));  
    
    CCMenu *menu=CCMenu::create(menuImage1,menuImage2, menuImage3,menuImage4, menuImage5, NULL);  
    menu->alignItemsVerticallyWithPadding(20);  
	menu->setPosition(size.width/2 - menuImage1->getContentSize().width / 2, 0);
	menu->setSwallowsTouches(true);
	
    this->addChild(menu,1);  
}

void GameNode::AddBridgePlank(CCObject *object)
{
	this->unscheduleUpdate();
	bridge.Stop();
	mode = Mode_Building;
	setPinBtnVisiable(true);
	bridge.SetEditMode(Bridge_EditMode_Structure);
}

void GameNode::AddBridgeRope(CCObject *object)
{
	this->unscheduleUpdate();
	bridge.Stop();
	mode = Mode_Building;
	setPinBtnVisiable(true);
	bridge.SetEditMode(Bridge_EditMode_Support);
}

void GameNode::AddBridgeCar(CCObject *object)
{
	if(mode == Mode_Testing)
	{
		bridge.SetEditMode(Bridge_EditMode_Car); 
		bridge.AddCar();
	}
}
void  GameNode::RevokeSlab(CCObject *object)
{
	if(mode != Mode_Testing)
	{
		bridge.RevokeSlab();
	}
}

void GameNode::TestBridgeStructure(CCObject *object)
{
	if (mode != Mode_Testing)
	{
		mode = Mode_Testing;
		setPinBtnVisiable(false);
		bridge.Start();
		this->scheduleUpdate(); 
	}
}

void GameNode::AddPinItems()
{
	CCSize size = CCDirector::getInstance()->getWinSize();

	int length =  size.width / PinMap::cols;
	int rows = (PinMap::rows + 1) / 2 ;
	int cols = (PinMap::cols + 1) / 2;
	for (int i = -rows + 1; i < rows; i++)
	{
		for (int j = -cols + 1; j < cols; j++)
		{
			Scale9Sprite *backgroundButton = Scale9Sprite::create("backA.png");
			Scale9Sprite *backgroundHighlightedButton = Scale9Sprite::create("backB.png");
			CCLabelTTF *titleButton = CCLabelTTF::create("O", "Marker Felt", 20);
			titleButton->setColor(ccc3(159, 168, 176));
			ControlButton *button = ControlButton::create(titleButton, backgroundButton);
			button->setBackgroundSpriteForState(backgroundHighlightedButton, Control::State::HIGH_LIGHTED);
			button->setTitleColorForState(ccWHITE,  Control::State::HIGH_LIGHTED);
			button->setAdjustBackgroundImage(true);
			button->setPosition(j * length, i * length);
			button->addTargetWithActionForControlEvents(this, cccontrol_selector(GameNode::onButtonTouchDown), Control::EventType::TOUCH_DOWN);

			this->addChild(button, 0);
			vPinBtn.pushBack(button);
		}
	}	
	leftPoint = CCPoint((-cols + 1) * length, 0);
	rightPoint = CCPoint((cols - 1) * length, 0);
	
}
