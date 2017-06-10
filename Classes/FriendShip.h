#ifndef __FRIEND_SHIP_H__
#define __FRIEND_SHIP_H__

#include "cocos2d.h"
using namespace std;
USING_NS_CC;
class FriendShip : public Layer {
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

	bool onConcactBegin(PhysicsContact& contact);
	bool onTouchBegan(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	void update(float dt);

	// implement the "static create()" method manually
	CREATE_FUNC(FriendShip);

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
	int percentageCat, percentageDog;
	float power;
	bool isDogAddingPower, isCatAddingPower;
	bool isBoneExist;

	// cat
	SpriteFrame* catNormal;
	SpriteFrame* catHit;
	SpriteFrame* catSick;
	Vector<SpriteFrame*> catHurted;
	Vector<SpriteFrame*> catBlast;

	// dog
	SpriteFrame* dogNormal;
	SpriteFrame* dogHit;
	SpriteFrame* dogSick;
	Vector<SpriteFrame*> dogHurted;
	Vector<SpriteFrame*> dogBlast;

	// tomato
	SpriteFrame* tomatoNormal;
	SpriteFrame* tomatoHurt1;
	SpriteFrame* tomatoHurt2;
	Animation* tomatoDie;
};

#endif // __FRIEND_SHIP_H__