#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

#include <iostream>
using namespace std;

//ACTOR (BASE) CLASS
class Actor: public GraphObject
{
public:
	Actor(int id, int x, int y, StudentWorld* myWorld, int dir, int depth, double size);

	//PURE VIRTUAL FUNCTIONS
	virtual void doSomething() = 0;
	virtual void bonk(Actor* bonker) = 0;

	//NON PURE VIRTUAL VOID FUNCTIONS
	virtual void damage() {}

	//IDENTIFIERS
	virtual bool isPeach() { return false; }
	virtual bool canBlockActors() { return false; }
	virtual bool canBeDamaged() { return false; }
	virtual bool isProjectile() { return false; }

	//ACCESSOR FUNCTIONS
	bool isAlive() { return life; }
	StudentWorld* getWorld() { return gameWorld; }

	//MUTATOR FUNCTIONS
	void setDead() { life = false; }
	void move(int pix, bool down);
	
private:
	bool life;
	StudentWorld* gameWorld;
};



//PEACH DECLARATIONS
class Peach : public Actor
{
public:
	Peach(int x, int y, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void doSomething();
	virtual void damage() { bonk(this); }
	virtual void bonk(Actor* bonker);
	virtual bool isPeach() { return true; }
	virtual bool canBeDamaged() { return true; }

	//MUTATOR FUNCTIONS
	void setHitpts(int pts) { hitPts = pts; }
	void setStarTicks(int ticks) { remainingStarTicks = ticks; }
	void giveShootPower() { firePower = true; }
	void giveJumpPower() { jumpPower = true; }

	//ACCESSOR FUNCTIONS
	bool hasStarPower() { return remainingStarTicks > 0; }
	bool hasShootPower() { return firePower; }
	bool hasJumpPower() { return jumpPower; }
private:
	int hitPts, time_to_recharge_before_next_fire, 
		remaining_jump_distance, remainingStarTicks, tempInvTicks;
	bool firePower, jumpPower;
};



//IDLEACTORS (BLOCKS AND PIPES) CLASS DECLARATIONS
class IdleActors : public Actor
{
public:
	IdleActors(int x, int y, int IID, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void doSomething() {}
	virtual void bonk(Actor* bonker) = 0;
	virtual bool canBlockActors() { return true; }
};

class Pipe : public IdleActors
{
public:
	Pipe(int x, int y, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void bonk(Actor* bonker) {}
};

class Block: public IdleActors
{
public:
	Block(int x, int y, StudentWorld* myWorld, bool isStar, bool isFlower, bool isShroom);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void bonk(Actor* bonker);
private:
	bool starBlock, flowerBlock, shroomBlock, beenBonked;
};



//FINISHLINE CLASS DECLARATIONS
class Finish : public Actor
{
public:
	Finish(int x, int y, int IID, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void doSomething();
	virtual void bonk(Actor* bonker) {}

	//PURE VIRTUAL FUNCTION TO COMPLETE TICK BASED ON SPECIFIC OBJECT
	virtual void complete() = 0;
};

class Flag : public Finish
{
public:
	Flag(int x, int y, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void complete() { getWorld()->setLevelComplete(); }
};

class Mario : public Finish
{
public:
	Mario(int x, int y, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void complete() { getWorld()->setGameComplete(); }
};



//GOODIE CLASS DECLARATIONS
class Goodie : public Actor
{
public:
	Goodie(int x, int y, int IID, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void doSomething();
	virtual void bonk(Actor* bonker) {}

	//PURE VIRTUAL FUNCTIONS TO INDICATE POINTS TO ADD AND WHAT POWER TO GIVE PEACH
	virtual int pointsAdded() = 0;
	virtual void givePeachPower() = 0;
};

class Flower : public Goodie
{
public:
	Flower(int x, int y, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual int pointsAdded() { return 50; }
	virtual void givePeachPower() { getWorld()->gamePeach()->giveShootPower(); }
};

class Mushroom : public Goodie
{
public:
	Mushroom(int x, int y, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual int pointsAdded() { return 75; }
	virtual void givePeachPower() { getWorld()->gamePeach()->giveJumpPower(); }
};

class Star : public Goodie
{
public:
	Star(int x, int y, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual int pointsAdded() { return 100; }
	virtual void givePeachPower() { getWorld()->gamePeach()->setStarTicks(150); }
};



//PROJECTILE CLASS DECLARATIONS
class Projectile : public Actor
{
public:
	Projectile(int x, int y, int dir, int IID, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void doSomething();
	virtual void bonk(Actor* bonker) {}
	virtual bool isProjectile() { return true; }

	//NON_PURE VIRTUAL FUNCTION TO CHECK FOR A CONDITION TO DAMAGE
	virtual bool conditionToDamage(Actor* damagedObject) { return damagedObject->canBeDamaged() && !(damagedObject->isPeach()); }
};

class PiranhaFireball : public Projectile
{
public:
	PiranhaFireball(int x, int y, int dir, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual bool conditionToDamage(Actor* damagedObject) { return damagedObject->isPeach(); }
};

class PeachFireball : public Projectile
{
public:
	PeachFireball(int x, int y, int dir, StudentWorld* myWorld);
};

class Shell : public Projectile
{
public:
	Shell(int x, int y, int dir, StudentWorld* myWorld);
};



//CLASS DECLARATION OF ENEMY CLASSES
class Enemy : public Actor
{
public:
	Enemy(int x, int y, int IID, int dir, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual void doSomething();
	virtual void bonk(Actor* bonker);
	virtual void damage();
	virtual bool canBeDamaged() { return true; }

	//NON-PURE VIRTUAL FUNCTION TO IDENTIFY KOOPA
	virtual bool isKoopa() { return false; }
private:
	void endLife();
};

class Goomba : public Enemy
{
public:
	Goomba(int x, int y, int dir, StudentWorld* myWorld);
};

class Koopa : public Enemy
{
public:
	Koopa(int x, int y, int dir, StudentWorld* myWorld);

	//IMPLEMENTATION OF VIRTUAL FUNCTIONS FROM BASE CLASS
	virtual bool isKoopa() { return true; }
};

class Piranha : public Enemy
{
public:
	Piranha(int x, int y, int dir, StudentWorld* myWorld);

	//OVERRIDE OF VIRTUAL FUNCTION FROM BASE CLASS
	virtual void doSomething();
private:
	int firingDelay;
};
#endif // ACTOR_H_
