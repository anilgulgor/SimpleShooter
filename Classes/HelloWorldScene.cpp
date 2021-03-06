#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Constants.h"
#include "GameObject.h"
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    m_visibleSize = Director::getInstance()->getVisibleSize();
    m_origin = Director::getInstance()->getVisibleOrigin();

	m_player = Player::create("CloseSelected.png");
	m_player->setPosition(Vec2(m_visibleSize.width / 2 + m_origin.x, m_visibleSize.height / 10 + m_origin.y));
	m_player->retain();
	this->addChild(m_player, 0);

	auto meteor = Meteorite::create("stone.png");
	meteor->setPosition(Vec2(m_origin.x + m_visibleSize.width / 2, m_origin.y+ m_visibleSize.height / 2));	
	this->addChild(meteor);


	m_gamePaused = true;

	auto enemy = Enemy::create("airplane.png");
	enemy->setRotation(180);
	enemy->setPosition(Vec2(m_origin.x + m_visibleSize.width /3, m_origin.y + m_visibleSize.height / 3));
	enemy->setDestinationPos(Vec2(m_origin.x + m_visibleSize.width / 3, m_origin.y + m_visibleSize.height / 3));
	this->addChild(enemy);

	
	m_label = Label::create("Press esc to begin", "Ariel", 16);
	m_label->setPosition(Vec2(m_origin.x + m_visibleSize.width / 2, m_origin.y + m_visibleSize.height / 2));
	m_label->retain();
	this->addChild(m_label);

	auto keyInputEvent = EventListenerKeyboard::create();
	keyInputEvent->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* eventPtr) {
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_ESCAPE:
			if (m_gamePaused) {
				resumeGame();				
				m_gamePaused = false;
			}
			else {
				pauseGame();
				m_gamePaused = true;
			}
			break;
		case EventKeyboard::KeyCode::KEY_ENTER:
		{
			if (m_player->isVisible())
				break;
			m_player->setVisible(true);
			removeAllChildrenWithCleanup(true);
			this->addChild(m_label);
			this->addChild(m_player);
			m_player->scheduleUpdate();
			resumeGame();
		}
		break;
		case EventKeyboard::KeyCode::KEY_BACKSPACE:
			Director::getInstance()->end();
			break;
		default:
			break;
		}
	};

	_eventDispatcher->addEventListenerWithFixedPriority(keyInputEvent, 20);
	
	schedule(schedule_selector(HelloWorld::spawnEnemy), 1.f);

	
	scheduleUpdate();


    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    

}

void HelloWorld::update(float delta)
{
	if (m_player->isVisible()==false) {
		pauseGame();
		m_label->setString("Game over. Press Enter to restart");
	}
}

void HelloWorld::spawnEnemy(float delta)
{
	float xPosition = random<float>(m_origin.x + 10, m_origin.x + m_visibleSize.width - 10);

	
	if ((int)xPosition % 2 == 0) {
		auto enemy = Enemy::create("airplane.png");
		enemy->setRotation(180);
		enemy->setPosition(Vec2(xPosition, m_origin.y + m_visibleSize.height));

		float endXPosition = random<float>(m_origin.x + 10, m_origin.x + m_visibleSize.width - 10);

		enemy->setDestinationPos(Vec2(endXPosition, m_origin.y - m_visibleSize.height));
		int projectiles = random<int>(3, 6);
		enemy->setBulletType(Projectile::Explosive, projectiles);
		this->addChild(enemy);
	}
	else
	{
		auto rock = Meteorite::create("stone.png");
		rock->setPosition(Vec2(xPosition, m_origin.y + m_visibleSize.height));
		this->addChild(rock);
	}


}

void HelloWorld::onEnter()
{
	Layer::onEnter();
	pauseGame();
}

void HelloWorld::onExit()
{


	Node::onExit();


}

void HelloWorld::pauseGame()
{
	for (const auto& it : this->getChildren()) {
		it->pause();
	}
	m_label->setString("Press esc to begin");
	m_label->setVisible(true);
	unschedule(schedule_selector(HelloWorld::spawnEnemy));

}

void HelloWorld::resumeGame()
{
	for (const auto& it : this->getChildren()) {
		it->resume();
	}
	m_label->setVisible(false);
	schedule(schedule_selector(HelloWorld::spawnEnemy), 1.f);
}

