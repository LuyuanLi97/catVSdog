#ifndef __FRIEND_SHIP_H__
#define __FRIEND_SHIP_H__

#include "cocos2d.h"
using namespace std;
USING_NS_CC;
class CatVsDog : public Layer {
public:
	static PhysicsWorld* world;
	static cocos2d::Scene* createScene();
	void setPhysicsWorld(PhysicsWorld * world);
	virtual bool init();

	// method
	void preloadMusic(); // 预加载音乐/音效
	void setFrame();
	void setAni();
	void setSprite(); // 设置出场效果
	void spriteFall();
	void addListener(); // 添加监听器
	void addBarsAndProps();
	void changeWind();

	float windstrength;

	bool onConcactBegin(PhysicsContact& contact);
	bool onTouchBegan(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	void update(float dt);
	void GameOver(string whoWins);
	void replayCallback(Ref * pSender);
	void exitCallback(Ref * pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(CatVsDog);

private:
	PhysicsWorld* m_world;
	Size visibleSize;
	Vec2 origin;

	Sprite* cat;
	Sprite* dog;
	Sprite* fish;
	Sprite* bone;
	Sprite* wind;
	list<PhysicsBody*> tomatoBodies;
	list<Sprite*> tomatoes;
	ProgressTimer* pCat;
	ProgressTimer* pDog;
	ProgressTimer* pCatPower;
	ProgressTimer* pDogPower;
	int percentageCat, percentageDog;
	int percentagePower;
	int power;
	int damage;
	bool isDogAddingPower, isCatAddingPower;
	bool isFishExist;
	bool isBoneExist;
	bool isCatDouble;
	bool isDogDouble;
	bool isCatBoom;
	bool isDogBoom;

	// cat
	SpriteFrame* catNormal;
	SpriteFrame* catHit;
	SpriteFrame* catSick;
	Vector<SpriteFrame*> catHurted;
	Vector<SpriteFrame*> catBlast;
	Vector<SpriteFrame*> catLaugh;

	// dog
	SpriteFrame* dogNormal;
	SpriteFrame* dogHit;
	SpriteFrame* dogSick;
	Vector<SpriteFrame*> dogHurted;
	Vector<SpriteFrame*> dogBlast;
	Vector<SpriteFrame*> dogLaugh;

	// tomato
	SpriteFrame* tomatoNormal;
	SpriteFrame* tomatoHurt1;
	SpriteFrame* tomatoHurt2;
	Vector<SpriteFrame*> tomatoDie;
	Vector<SpriteFrame*> tomatoHurted;

	// wind
	SpriteFrame* wind_left_strong;
	SpriteFrame* wind_left_weak;
	SpriteFrame* wind_right_strong;
	SpriteFrame* wind_right_weak;
	cocos2d::Label* windLevel;
	cocos2d::Label* turn;


	string lastAttack;
};

#endif // __FRIEND_SHIP_H__

