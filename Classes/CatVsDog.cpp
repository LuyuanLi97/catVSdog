#pragma execution_character_set("utf-8")
#include "CatVsDog.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void CatVsDog::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* CatVsDog::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();

  scene->getPhysicsWorld()->setAutoStep(false);

  // Debug 模式
  // 开启debug模式需要setAutoStep(true) 并注释掉update函数第一行
  //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

  scene->getPhysicsWorld()->setGravity(Vec2(0, -1000.0f));
  auto layer = CatVsDog::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  scene->addChild(layer);
  return scene;
}

// on "init" you need to initialize your instance
bool CatVsDog::init() {

	if (!Layer::init()) {
		return false;
	}
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	// 预加载bgm和音效
	preloadMusic();
	// 添加各种frame
	setFrame();
	setAni();
	// 加载背景、地面并设置刚体
	setSprite();
	// 添加监听器
	addListener();
	// 开场设置，包含番茄、猫、狗的掉落
	spriteFall();

	// 添加调度器
	schedule(schedule_selector(CatVsDog::update), 0.01f, kRepeatForever, 0.1f);

	//power set to 0
	power = 0;
	damage = 10;

	isBoneExist = false;
	isFishExist = false;
	isCatAddingPower = false;
	isDogAddingPower = false;
	isDogDouble = false;
	isCatDouble = false;
	isDogBoom = false;
	isCatBoom = false;
	lastAttack = "cat";

	// 添加血条和道具
	addBarsAndProps();
	// 改变风向
	changeWind();
	return true;
}

// 预加载音效
void CatVsDog::preloadMusic() {
  auto sae = SimpleAudioEngine::getInstance();
  sae->preloadEffect("catMeow.wav");
  sae->preloadEffect("dogBark.wav");
  sae->preloadEffect("tomatoHurt.mp3");
  sae->preloadBackgroundMusic("bgm.mp3");
  sae->preloadEffect("gameover.mp3");
  sae->preloadEffect("clickItem.mp3");
  sae->preloadEffect("boom.wav");
  sae->playBackgroundMusic("bgm.mp3", true);
}

// 添加猫、狗和番茄的帧
void CatVsDog::setFrame() {
	// cat
	auto texture = Director::getInstance()->getTextureCache()->addImage("/cat/cat_normal.png");
	catNormal = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 115, 160)));
	texture = Director::getInstance()->getTextureCache()->addImage("/cat/cat_hit.png");
	catHit = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 115, 160)));
	texture = Director::getInstance()->getTextureCache()->addImage("/cat/cat_sick.png");
	catSick = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 115, 160)));

	// dog
	texture = Director::getInstance()->getTextureCache()->addImage("/dog/dog_normal.png");
	dogNormal = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 130, 185)));
	texture = Director::getInstance()->getTextureCache()->addImage("/dog/dog_hit.png");
	dogHit = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 130, 185)));
	texture = Director::getInstance()->getTextureCache()->addImage("/dog/dog_sick.png");
	dogSick = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 130, 185)));

	// tomato
	texture = Director::getInstance()->getTextureCache()->addImage("/tomato/tomato_normal.png");
	tomatoNormal = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 55, 55)));
	texture = Director::getInstance()->getTextureCache()->addImage("/tomato/tomato_hurt1.png");
	tomatoHurt1 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 55, 55)));
	texture = Director::getInstance()->getTextureCache()->addImage("/tomato/tomato_hurt2.png");
	tomatoHurt2 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 55, 55)));
}

// 加载各种动画 
void CatVsDog::setAni() {
	dogHurted.reserve(3);
	for (int i = 0; i < 3; i++) {
		string filePath = "/dog/dog_hurted" + to_string(i) + ".png";
		dogHurted.pushBack(SpriteFrame::create(filePath, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 130, 185))));
	}

	dogBlast.reserve(3);
	for (int i = 0; i < 3; i++) {
		string filePath = "/dog/dog_blast" + to_string(i) + ".png";
		dogBlast.pushBack(SpriteFrame::create(filePath, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 130, 185))));
	}

	dogLaugh.reserve(3);
	for (int i = 0; i < 3; i++) {
		dogLaugh.pushBack(dogHit);
	}

	catHurted.reserve(3);
	for (int i = 0; i < 3; i++) {
		string filePath = "/cat/cat_hurted" + to_string(i) + ".png";
		catHurted.pushBack(SpriteFrame::create(filePath, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 115, 160))));
	}

	catBlast.reserve(3);
	for (int i = 0; i < 3; i++) {
		string filePath = "/cat/cat_blast" + to_string(i) + ".png";
		catBlast.pushBack(SpriteFrame::create(filePath, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 115, 160))));
	}

	catLaugh.reserve(3);
	for (int i = 0; i < 3; i++) {
		catLaugh.pushBack(catHit);
	}

	tomatoHurted.reserve(2);
	for (int i = 1; i < 3; i++) {
		string filePath = "/tomato/tomato_hurt" + to_string(i) + ".png";
		tomatoHurted.pushBack(SpriteFrame::create(filePath, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 55, 55))));
	}

	tomatoDie.reserve(6);
	for (int i = 1; i < 7; i++) {
		string filePath = "/tomato/die_ani" + to_string(i) + ".png";
		tomatoDie.pushBack(SpriteFrame::create(filePath, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 55, 55))));
	}

}

void CatVsDog::setSprite() {
	// add backgroud
	visibleSize = Director::getInstance()->getVisibleSize();
	auto bg = Sprite::create("bg1.png");
	//bg->setPosition(visibleSize / 2);
	bg->setPosition(visibleSize / 2);
	bg->setScale(visibleSize.width / bg->getContentSize().width, visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);

	// add ground
	// set tag 1
	auto ground = Sprite::create("ground.png");
	ground->setScale(visibleSize.width / ground->getContentSize().width, 1);
	ground->setPosition(visibleSize.width / 2, ground->getContentSize().height / 2);
	auto groundBody = PhysicsBody::createBox(ground->getContentSize(), PhysicsMaterial(500.0f, 0.0f, 1.0f));
	// 与任何物体碰撞，且事件触发
	groundBody->setCategoryBitmask(0xFFFFFFFF);
	groundBody->setCollisionBitmask(0xFFFFFFFF);
	groundBody->setContactTestBitmask(0x00000000);
	groundBody->setDynamic(false);
	groundBody->setTag(1);
	ground->setPhysicsBody(groundBody);
	this->addChild(ground, 1);
}

// 添加监听器
void CatVsDog::addListener() {
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(CatVsDog::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(CatVsDog::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 创建角色
void CatVsDog::spriteFall() {
	// add tomatoes randomly
	// set tag 2
	for (int i = 0; i < 8; i++) {
		auto tomato = Sprite::createWithSpriteFrame(tomatoNormal);
		tomato->setScale(random(0.8, 2.5));
		tomato->setPosition(rand() % (int)(visibleSize.width - 100) + 60, visibleSize.height);
		auto tomatoBody = PhysicsBody::createBox(tomato->getContentSize(), PhysicsMaterial(50.f, 0.2f, 10.0f));
		tomatoBody->setCategoryBitmask(0x00000002); // 0010
		tomatoBody->setCollisionBitmask(0x00000001);
		tomatoBody->setContactTestBitmask(0x00000001);
		tomatoBody->setTag(2);
		tomatoBody->setRotationEnable(false);
		tomato->setPhysicsBody(tomatoBody);
		tomatoes.push_back(tomato);
		tomatoBodies.push_back(tomatoBody);
		this->addChild(tomato, 2);
	}

	// add cat
	cat = Sprite::createWithSpriteFrame(catNormal);
	cat->setPosition(visibleSize.width / 4, visibleSize.height / 2);
	auto catBody = PhysicsBody::createBox(cat->getContentSize(), PhysicsMaterial(200.f, 0.0f, 10.0f));
	// 猫和番茄不发生碰撞
	catBody->setCategoryBitmask(0x00000006); // 0110
	catBody->setCollisionBitmask(0x00000006);
	catBody->setContactTestBitmask(0x00000006);
	catBody->setRotationEnable(false);
	cat->setPhysicsBody(catBody);
	this->addChild(cat, 1);

	// add dog
	dog = Sprite::createWithSpriteFrame(dogNormal);
	dog->setPosition(3 * visibleSize.width / 4, visibleSize.height / 2);
	auto dogBody = PhysicsBody::createBox(cat->getContentSize(), PhysicsMaterial(200.f, 0.0f, 10.0f));
	dogBody->setCategoryBitmask(0x00000006); // 0110
	dogBody->setCollisionBitmask(0x00000006);
	dogBody->setContactTestBitmask(0x00000001); // 0001
	dogBody->setRotationEnable(false);
	dog->setPhysicsBody(dogBody);
	this->addChild(dog, 1);
}

// 添加血条蓄力条和道具
void CatVsDog::addBarsAndProps() {
	//add wind
	wind = Sprite::create("/wind/wind_left_weak.png");
	wind->setPosition(visibleSize.width / 2, visibleSize.height / 5 * 4);
	wind->setName("windLogo");
	windLevel = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 30);
	windLevel->setPosition(Vec2(wind->getPositionX() + 5, wind->getPositionY() - 60));
	turn = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 30);
	turn->setPosition(Vec2(wind->getPositionX() + 5, wind->getPositionY() - 100));
	this->addChild(wind, 1);
	this->addChild(windLevel, 1);
	this->addChild(turn, 1);

	//血条
	Sprite* sp0 = Sprite::create("blood_state.png");
	sp0->setPosition(visibleSize.width / 2, visibleSize.height - sp0->getContentSize().height / 2);
	this->addChild(sp0, 0);

	//猫的血条
	Sprite* sp = Sprite::create("blood.jpg");
	percentageCat = 100;
	pCat = ProgressTimer::create(sp);
	pCat->setType(ProgressTimerType::BAR);
	pCat->setBarChangeRate(Point(1, 0));
	pCat->setMidpoint(Point(1, 0.5));
	pCat->setPercentage(100);
	pCat->setPosition(Vec2(visibleSize.width / 2 - 232, visibleSize.height - sp->getContentSize().height - 28));
	this->addChild(pCat, 1);

	//狗的血条
	percentageDog = 100;
	pDog = ProgressTimer::create(sp);
	pDog->setType(ProgressTimerType::BAR);
	pDog->setBarChangeRate(Point(1, 0));
	pDog->setMidpoint(Point(0, 0.5));
	pDog->setPercentage(100);
	pDog->setPosition(Vec2(visibleSize.width / 2 + 238, visibleSize.height - sp->getContentSize().height - 28));
	this->addChild(pDog, 1);

	//	猫蓄力条
	sp = Sprite::create("power_tri.png");
	sp->setFlippedX(true);
	percentagePower = 0;
	pCatPower = ProgressTimer::create(sp);
	pCatPower->setType(ProgressTimerType::BAR);
	pCatPower->setBarChangeRate(Point(1, 0));
	pCatPower->setMidpoint(Point(0, 0.5));
	pCatPower->setPercentage(0);
	pCatPower->setPosition(cat->getPosition() + Vec2(0, -100));
	this->addChild(pCatPower, 1);

	//	狗蓄力条
	sp = Sprite::create("power_tri.png");
	pDogPower = ProgressTimer::create(sp);
	pDogPower->setType(ProgressTimerType::BAR);
	pDogPower->setBarChangeRate(Point(1, 0));
	pDogPower->setMidpoint(Point(0, 0.5));
	pDogPower->setPercentage(0);
	pDogPower->setPosition(dog->getPosition() + Vec2(0, -100));
	this->addChild(pDogPower, 1);

	//	狗血瓶
	auto dogBloodVialItem = MenuItemImage::create(
		"add_blood_normal.jpg",
		"add_blood_click.jpg",
		[&](Ref* sender) {
		if (lastAttack == "dog") return;
		lastAttack = "dog";
		changeWind();
		percentageDog += 30;
		if (percentageDog > 100) percentageDog = 100;
		pDog->setPercentage(percentageDog);
		this->removeChildByName("dogBloodVialMenu");
		SimpleAudioEngine::getInstance()->playEffect("clickItem.mp3", false);
	});
	auto dogBloodVialMenu = Menu::create(dogBloodVialItem, NULL);
	dogBloodVialMenu->setPosition(dog->getPosition() + Vec2(0, 200));
	dogBloodVialMenu->setName("dogBloodVialMenu");
	this->addChild(dogBloodVialMenu, 1);

	//	猫血瓶
	auto catBloodVialItem = MenuItemImage::create(
		"add_blood_normal.jpg",
		"add_blood_click.jpg",
		[&](Ref* sender) {
		if (lastAttack == "cat") return;
		lastAttack = "cat";
		changeWind();
		percentageCat += 30;
		if (percentageCat > 100) percentageCat = 100;
		pCat->setPercentage(percentageCat);
		this->removeChildByName("catBloodVialMenu");
		SimpleAudioEngine::getInstance()->playEffect("clickItem.mp3", false);
	});
	auto catBloodVialMenu = Menu::create(catBloodVialItem, NULL);
	catBloodVialMenu->setPosition(cat->getPosition() + Vec2(0, 200));
	catBloodVialMenu->setName("catBloodVialMenu");
	this->addChild(catBloodVialMenu, 1);

	//	狗*2
	auto dogDoubleBoneItem = MenuItemImage::create(
		"double_button_normal.png",
		"double_button_click.png",
		[&](Ref* sender) {
		if (lastAttack == "dog") return;
		isDogDouble = true;
		this->removeChildByName("dogDoubleBoneMenu");
		SimpleAudioEngine::getInstance()->playEffect("clickItem.mp3", false);

	});
	auto dogDoubleBoneMenu = Menu::create(dogDoubleBoneItem, NULL);
	dogDoubleBoneMenu->setPosition(dog->getPosition() + Vec2(-100, 200));
	dogDoubleBoneMenu->setName("dogDoubleBoneMenu");
	this->addChild(dogDoubleBoneMenu, 1);

	//	猫*2
	auto catDoubleBoneItem = MenuItemImage::create(
		"double_button_normal.png",
		"double_button_click.png",
		[&](Ref* sender) {
		if (lastAttack == "cat") return;
		isCatDouble = true;
		this->removeChildByName("catDoubleBoneMenu");
		SimpleAudioEngine::getInstance()->playEffect("clickItem.mp3", false);
	});
	auto catDoubleBoneMenu = Menu::create(catDoubleBoneItem, NULL);
	catDoubleBoneMenu->setPosition(cat->getPosition() + Vec2(100, 200));
	catDoubleBoneMenu->setName("catDoubleBoneMenu");
	this->addChild(catDoubleBoneMenu, 1);

	//	狗炸弹
	auto dogBoomItem = MenuItemImage::create(
		"boomer_normal.png",
		"boomer_click.png",
		[&](Ref* sender) {
		if (lastAttack == "dog") return;
		isDogBoom = true;
		this->removeChildByName("dogBoomMenu");
		SimpleAudioEngine::getInstance()->playEffect("clickItem.mp3", false);
	});
	auto dogBoomMenu = Menu::create(dogBoomItem, NULL);
	dogBoomMenu->setPosition(dog->getPosition() + Vec2(-200, 200));
	dogBoomMenu->setName("dogBoomMenu");
	this->addChild(dogBoomMenu, 1);

	//	猫炸弹
	auto catBoomItem = MenuItemImage::create(
		"boomer_normal.png",
		"boomer_click.png",
		[&](Ref* sender) {
		if (lastAttack == "cat") return;
		isCatBoom = true;
		this->removeChildByName("catBoomMenu");
		SimpleAudioEngine::getInstance()->playEffect("clickItem.mp3", false);
	});
	auto catBoomMenu = Menu::create(catBoomItem, NULL);
	catBoomMenu->setPosition(cat->getPosition() + Vec2(200, 200));
	catBoomMenu->setName("catBoomMenu");
	this->addChild(catBoomMenu, 1);
}

void CatVsDog::changeWind() {
	int a = 10 + rand() % 1000;
	windstrength = (float)a / 100;
	a = 0 + rand() % 2;
	if (a == 0)
		windstrength = -windstrength;
	if (0 < windstrength && windstrength < 5.0) {
		this->removeChildByName("windLogo");
		wind = Sprite::create("/wind/wind_right_weak.png");
		wind->setPosition(visibleSize.width / 2, visibleSize.height / 5 * 4);
		wind->setName("windLogo");
		this->addChild(wind);
		char* temp = "Cat'turn";
		if (lastAttack == "cat") {
			temp = "Dog's turn";
		}
		CCString* level = CCString::createWithFormat("%.1f", windstrength);
		windLevel->setString(level->getCString());
		turn->setString(temp);
	}
	else if (windstrength >= 5.0) {
		this->removeChildByName("windLogo");
		wind = Sprite::create("/wind/wind_right_strong.png");
		wind->setPosition(visibleSize.width / 2, visibleSize.height / 5 * 4);
		wind->setName("windLogo");
		this->addChild(wind);
		char* temp = "Cat'turn";
		if (lastAttack == "cat") {
			temp = "Dog's turn";
		}
		CCString* level = CCString::createWithFormat("%.1f", windstrength);
		windLevel->setString(level->getCString());
		turn->setString(temp);
	}
	else if (-5.0 < windstrength && windstrength <= 0) {
		this->removeChildByName("windLogo");
		wind = Sprite::create("/wind/wind_left_weak.png");
		wind->setPosition(visibleSize.width / 2, visibleSize.height / 5 * 4);
		wind->setName("windLogo");
		this->addChild(wind);
		char* temp = "Cat'turn";
		if (lastAttack == "cat") {
			temp = "Dog's turn";
		}
		CCString* level = CCString::createWithFormat("%.1f", windstrength);
		windLevel->setString(level->getCString());
		turn->setString(temp);
	}
	else {
		this->removeChildByName("windLogo");
		wind = Sprite::create("/wind/wind_left_strong.png");
		wind->setPosition(visibleSize.width / 2, visibleSize.height / 5 * 4);
		wind->setName("windLogo");
		this->addChild(wind);

		char* temp = "Cat'turn";
		if (lastAttack == "cat") {
			temp = "Dog's turn";
		}
		CCString* level = CCString::createWithFormat("%.1f", windstrength);
		windLevel->setString(level->getCString());
		turn->setString(temp);
	}
}

bool CatVsDog::onTouchBegan(Touch *touch, Event *event) {
	Vec2 pos = touch->getLocation();
	//若点到狗
	if (dog->getBoundingBox().containsPoint(pos) && lastAttack == "cat") {
		//狗蓄力
		isDogAddingPower = true;
	}
	if (cat->getBoundingBox().containsPoint(pos) && lastAttack == "dog") {
		isCatAddingPower = true;
	}
	return true;
}

void CatVsDog::onTouchEnded(Touch *touch, Event *event) {
	if (isDogAddingPower == true) {
		if (isDogDouble) {
			bone = Sprite::create("double_bone.png");
			isDogDouble = false;
			damage = 20;
		}
		else if (isDogBoom) {
			bone = Sprite::create("boom.png");
			isDogBoom = false;
			damage = 30;
		}
		else {
			bone = Sprite::create("bone.png");
		}
		auto boneBody = PhysicsBody::createBox(bone->getContentSize(), PhysicsMaterial(20.0f, 0.5f, 1.0f));
		boneBody->setCategoryBitmask(0x00000007);
		boneBody->setCollisionBitmask(0x00000001);
		boneBody->setContactTestBitmask(0x00000001);
		boneBody->setVelocity(Vec2(-10 * power + windstrength * 20, 10 * power));
		bone->setPhysicsBody(boneBody);
		bone->setPosition(3 * visibleSize.width / 4 - 100, 300);
		this->addChild(bone, 1);
		isBoneExist = true;
		power = 0;
		isDogAddingPower = false;
		lastAttack = "dog";
		pDogPower->setPercentage(0);
	}
	else if (isCatAddingPower == true) {
		if (isCatDouble) {
			fish = Sprite::create("double_fish.png");
			isCatDouble = false;
			damage = 20;
		}
		else if (isCatBoom) {
			fish = Sprite::create("boom.png");
			isCatBoom = false;
			damage = 30;
		}
		else {
			fish = Sprite::create("fish.png");
		}
		auto fishBody = PhysicsBody::createBox(fish->getContentSize(), PhysicsMaterial(20.0f, 0.5f, 1.0f));
		fishBody->setCategoryBitmask(0x00000007);
		fishBody->setCollisionBitmask(0x00000001);
		fishBody->setContactTestBitmask(0x00000001);
		fishBody->setTag(5);
		fish->setPhysicsBody(fishBody);
		fish->setPosition(visibleSize.width / 4 + 100, 300);
		fishBody->setVelocity(Vec2(10 * power + windstrength * 20, 10 * power));
		this->addChild(fish, 1);
		isFishExist = true;
		power = 0;
		isCatAddingPower = false;
		lastAttack = "cat";
		pCatPower->setPercentage(0);
	}
}

void CatVsDog::update(float dt) {
	this->getScene()->getPhysicsWorld()->step(1 / 100.0f);
	//猫狗蓄力
	if (isDogAddingPower == true) {
		power = (power + 1) % 101;
		pDogPower->setPercentage(power);
	} else if (isCatAddingPower == true) {
		power = (power + 1) % 101;
		pCatPower->setPercentage(power);
	}
	//检测骨头是否砸到猫
	if (isBoneExist) {
		auto boneRec = bone->getBoundingBox();
		auto catRec = cat->getBoundingBox();
		//骨头砸到猫
		if (catRec.containsPoint(bone->getPosition())) {
			isBoneExist = false;
			bone->removeFromParent();
			if (damage == 10 || damage == 20) {
				auto catHurtedAnimation = Animation::createWithSpriteFrames(catHurted, 0.3f);
				catHurtedAnimation->setRestoreOriginalFrame(true);
				auto catHurtedAnimate = Animate::create(catHurtedAnimation);
				cat->runAction(Repeat::create(catHurtedAnimate, 1));
			}
			else {
				auto catBlastAnimation = Animation::createWithSpriteFrames(catBlast, 0.3f);
				catBlastAnimation->setRestoreOriginalFrame(true);
				auto catBlastAnimate = Animate::create(catBlastAnimation);
				cat->runAction(Repeat::create(catBlastAnimate, 1));
				SimpleAudioEngine::getInstance()->playEffect("boom.wav", false);
			}
			auto dogLaughAnimation = Animation::createWithSpriteFrames(dogLaugh, 0.3f);
			dogLaughAnimation->setRestoreOriginalFrame(true);
			auto dogLaughAnimate = Animate::create(dogLaughAnimation);
			dog->runAction(Repeat::create(dogLaughAnimate, 1));
			SimpleAudioEngine::getInstance()->playEffect("catMeow.wav", false);
			
			percentageCat -= damage;
			if (percentageCat < 0)
				percentageCat = 0;
			pCat->setPercentage(percentageCat);
			if (percentageCat == 0) {
				//狗胜利:狗这边放烟花,猫那边有灰色烟雾
				auto firework = CCParticleFireworks::create();
				firework->setPosition(3 * visibleSize.width / 4 + 50, visibleSize.height / 2 - 300);
				this->addChild(firework, 1);

				auto smoke = CCParticleSmoke::create();
				smoke->setPosition(visibleSize.width / 4, visibleSize.height / 2 - 300);
				smoke->setColor(Color3B(96, 96, 96));
				this->addChild(smoke, 1);

				GameOver("dog");
			}
			damage = 10;
			changeWind();
		}
		//骨头没有砸到猫，且速度为0时， 消失
		else if (abs(bone->getPhysicsBody()->getVelocity().x - 0.0f) < 0.000001f 
			|| bone->getPositionX() >= visibleSize.width || bone->getPositionX() < 0) {
			if(bone != nullptr)
				bone->removeFromParent();

			isBoneExist = false;
			damage = 10;
			changeWind();
		}
	}
	//检测鱼是否砸到狗
	else if (isFishExist) {
		auto dogRec = dog->getBoundingBox();
		//鱼砸到狗
		if (dogRec.containsPoint(fish->getPosition())) {
			if(fish != nullptr)
				fish->removeFromParent();
			isFishExist = false;
			if (damage == 10 || damage == 20) {
				auto dogHurtedAnimation = Animation::createWithSpriteFrames(dogHurted, 0.3f);
				dogHurtedAnimation->setRestoreOriginalFrame(true);
				auto dogHurtedAnimate = Animate::create(dogHurtedAnimation);
				dog->runAction(Repeat::create(dogHurtedAnimate, 1));
			}
			else {
				auto dogBlastAnimation = Animation::createWithSpriteFrames(dogBlast, 0.3f);
				dogBlastAnimation->setRestoreOriginalFrame(true);
				auto dogBlastAnimate = Animate::create(dogBlastAnimation);
				dog->runAction(Repeat::create(dogBlastAnimate, 1));
				SimpleAudioEngine::getInstance()->playEffect("boom.wav", false);
			}
			auto catLaughAnimation = Animation::createWithSpriteFrames(catLaugh, 0.3f);
			catLaughAnimation->setRestoreOriginalFrame(true);
			auto catLaughAnimate = Animate::create(catLaughAnimation);
			cat->runAction(Repeat::create(catLaughAnimate, 1));
			SimpleAudioEngine::getInstance()->playEffect("dogBark.wav", false);

			percentageDog -= damage;
			if (percentageDog < 0)
				percentageDog = 0;
			pDog->setPercentage(percentageDog);
			if (percentageDog == 0) {
				//猫胜利：猫这边放烟花，狗那边有灰色烟雾
				auto firework = CCParticleFireworks::create();
				firework->setPosition(visibleSize.width / 4 - 50, visibleSize.height / 2 - 300);
				this->addChild(firework, 1);

				auto smoke = CCParticleSmoke::create();
				smoke->setPosition(3 * visibleSize.width / 4, visibleSize.height / 2 - 300);
				smoke->setColor(Color3B(96, 96, 96));
				this->addChild(smoke, 1);
				GameOver("cat");
			}
			damage = 10;
			changeWind();
		}
		//鱼没有砸到狗，且速度为0时， 消失
		else if (abs(fish->getPhysicsBody()->getVelocity().x - 0.0f) < 0.000001f 
			|| fish->getPositionX() >= visibleSize.width || fish->getPositionX() <= 0) {

			isFishExist = false;
			if(fish != nullptr)
				fish->removeFromParent();
			damage = 10;
			changeWind();
		}
	}

	//检测骨头是否砸到番茄
	if (isBoneExist) {
		for (list<Sprite*>::iterator iter = tomatoes.begin(); iter != tomatoes.end(); iter++) {
			auto tomatoRec = (*iter)->getBoundingBox();
			if (tomatoRec.containsPoint(bone->getPosition())) {
				SimpleAudioEngine::getInstance()->playEffect("tomatoHurt.mp3", false);

				if ((*iter)->isFrameDisplayed(tomatoNormal)) {
					auto tomatoHurtedAnimation = Animation::createWithSpriteFrames(tomatoHurted, 0.1f);
					auto tomatoHurtedAnimate = Animate::create(tomatoHurtedAnimation);
					(*iter)->runAction(Repeat::create(tomatoHurtedAnimate, 1));
				}
				else if ((*iter)->isFrameDisplayed(tomatoHurted.back())) {
					auto tomatoDieAnimation = Animation::createWithSpriteFrames(tomatoDie, 0.1f);
					auto tomatoDieAnimate = Animate::create(tomatoDieAnimation);
					Sequence* seq = Sequence::create(tomatoDieAnimate, CallFunc::create(CC_CALLBACK_0
						(Sprite::removeFromParent, (*iter))), NULL);
					(*iter)->runAction(seq);

					iter = tomatoes.erase(iter);
				}
				if(bone != nullptr)
					bone->removeFromParent();
				isBoneExist = false;
				damage = 10;
				changeWind();
				break;
			}
		}
	}
	//检测鱼砸番茄
	else if (isFishExist) {
		for (list<Sprite*>::iterator iter = tomatoes.begin(); iter != tomatoes.end(); iter++) {
			auto tomatoRec = (*iter)->getBoundingBox();
			if (tomatoRec.containsPoint(fish->getPosition())) {
				SimpleAudioEngine::getInstance()->playEffect("tomatoHurt.mp3", false);

				if ((*iter)->isFrameDisplayed(tomatoNormal)) {
					auto tomatoHurtedAnimation = Animation::createWithSpriteFrames(tomatoHurted, 0.1f);
					auto tomatoHurtedAnimate = Animate::create(tomatoHurtedAnimation);
					(*iter)->runAction(Repeat::create(tomatoHurtedAnimate, 1));
				}
				else if ((*iter)->isFrameDisplayed(tomatoHurted.back())) {
					auto tomatoDieAnimation = Animation::createWithSpriteFrames(tomatoDie, 0.1f);
					auto tomatoDieAnimate = Animate::create(tomatoDieAnimation);
					Sequence* seq = Sequence::create(tomatoDieAnimate, CallFunc::create(CC_CALLBACK_0
					(Sprite::removeFromParent, (*iter))), NULL);
					(*iter)->runAction(seq);

					iter = tomatoes.erase(iter);
				}
				if(fish != nullptr)
					fish->removeFromParent();

				isFishExist = false;
				damage = 10;
				changeWind();
				break;
			}
		}
	}
}

void CatVsDog::GameOver(string whoWins) {
	unschedule(schedule_selector(CatVsDog::update));
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
	SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

	auto label1 = Label::create();
	if (whoWins == "cat")
		label1 = Label::createWithTTF("CAT WINS!", "fonts/Marker Felt.ttf", 60);
	else 
		label1 = Label::createWithTTF("DOG WINS!", "fonts/Marker Felt.ttf", 60);
	label1->setColor(Color3B(0, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("REPLAY", "fonts/Marker Felt.ttf", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(CatVsDog::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);

	auto label3 = Label::createWithTTF("EXIT", "fonts/Marker Felt.ttf", 40);
	label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(CatVsDog::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
}

void CatVsDog::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(CatVsDog::createScene());
}

void CatVsDog::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

