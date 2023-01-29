#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"

//ACTOR (BASE CLASS) IMPLEMENTATIONS
Actor::Actor(int id, int x, int y, StudentWorld* myWorld, 
	int dir, int depth, double size):
	GraphObject(id, x, y, dir, depth, size), life(true), gameWorld(myWorld) {}

void Actor::move(int pix, bool down)
{
	Actor* placeholder;
	if (down && (!getWorld()->objectBlocking(getX(), getY() - pix, this, placeholder) || !placeholder->canBlockActors()))
		moveTo(getX(), getY() - pix);

	if (getDirection() == 0 && (!getWorld()->objectBlocking(getX() + pix, getY(), this, placeholder) || !placeholder->canBlockActors()))
		moveTo(getX() + pix, getY());
	else if (getDirection() == 180 && (!getWorld()->objectBlocking(getX() - pix, getY(), this, placeholder) || !placeholder->canBlockActors()))
		moveTo(getX() - pix, getY());
	else if (isProjectile())
		setDead();
	else
		setDirection((int)abs(getDirection() - 180));
}

//PEACH IMPLEMENTATIONS
Peach::Peach(int x, int y, StudentWorld* myWorld): 
	Actor(IID_PEACH, x, y, myWorld, 0, 0, 1.0), hitPts(1),
	firePower(false), jumpPower(false), remaining_jump_distance(0), remainingStarTicks(0), tempInvTicks(0), time_to_recharge_before_next_fire(0){}


void Peach::doSomething()
{
	//Do nothing if peach is dead
	if (!isAlive())
		return;

	//If peach has starpower, decrement remaining ticks
	if (remainingStarTicks > 0)
		remainingStarTicks--;

	//If peach has temporary invincibility, decrement remaining ticks
	if (tempInvTicks > 0)
		tempInvTicks--;

	//If there is firepower delay, decrement remaining ticks
	if (time_to_recharge_before_next_fire > 0)
		time_to_recharge_before_next_fire--;

	Actor* overlap;
	//Bonk overlaping object if one exists
	if (getWorld()->objectBlocking(getX(), getY(), this, overlap))
		overlap->bonk(this);

	//Peach is in process of jumping
	if (remaining_jump_distance > 0) 
	{
		if (getWorld()->objectBlocking(getX(), getY() + 4, this, overlap)  && overlap->canBlockActors())
		{
			overlap->bonk(this);
			remaining_jump_distance = 0;
		}
		else
		{
			moveTo(getX(), getY() + 4);
			remaining_jump_distance--;
		}
	}
	//Check if Peach is falling and move down if necessary
	else if((!getWorld()->objectBlocking(getX(), getY(), this, overlap) || !overlap->canBlockActors()) && 
		(!getWorld()->objectBlocking(getX(), getY() - 1, this, overlap) || !overlap->canBlockActors()) && 
		(!getWorld()->objectBlocking(getX(), getY() - 2, this, overlap) || !overlap->canBlockActors()) && 
		(!getWorld()->objectBlocking(getX(), getY() - 3, this, overlap) || !overlap->canBlockActors()))
		moveTo(getX(), getY() - 4);


	int moveNum; //Check for any key stroke and alter Peach appropriately
	if (getWorld()->getKey(moveNum))
	{
		switch (moveNum)
		{
		case KEY_PRESS_LEFT: //Left Direction Key Pressed
			setDirection(180);
			if (!getWorld()->objectBlocking(getX() - 4, getY(), this, overlap)  || !overlap->canBlockActors())
				moveTo(getX() - 4, getY());
			break;
		case KEY_PRESS_RIGHT: //Right Direction Key Pressed
			setDirection(0);
			if (!getWorld()->objectBlocking(getX() + 4, getY(), this, overlap) || !overlap->canBlockActors())
				moveTo(getX() + 4, getY());
			break;
		case KEY_PRESS_SPACE: //Space Key Pressed
			if (!firePower)//Check for Fire Power
			{
				return;
			}
			else if (time_to_recharge_before_next_fire > 0) //Check for fire delay
			{
				return;
			}
			else
			{
				//Fire new Peach Fireball
				time_to_recharge_before_next_fire = 8;
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				if(getDirection() == 0)
					getWorld()->addActor(new PeachFireball((int)getX() + 4, (int)getY(), getDirection(), getWorld()));
				else
					getWorld()->addActor(new PeachFireball((int)getX() - 4, (int)getY(), getDirection(), getWorld()));
			}
			break;
		case KEY_PRESS_UP: //Up Key Pressed
			if (getWorld()->objectBlocking(getX(), getY() - 1, this, overlap) && overlap->canBlockActors())
			{
				if (jumpPower) //Check for jump power
					remaining_jump_distance = 12;
				else
					remaining_jump_distance = 8;

				getWorld()->playSound(SOUND_PLAYER_JUMP);
			}
			break;
		}
	}
}

void Peach::bonk(Actor* bonker)
{
	//Do nothing to Peach if she has starPower
	if (remainingStarTicks > 0 || tempInvTicks > 0)
		return;

	hitPts--;
	tempInvTicks = 10;
	firePower = false;
	jumpPower = false;

	//Set Peach dead if she has no more hit points
	if (hitPts >= 1)
		getWorld()->playSound(SOUND_PLAYER_HURT);
	else
		setDead();
}

//BLOCKING CLASSES
IdleActors::IdleActors(int x, int y, int IID, StudentWorld* myWorld) : Actor(IID, x, y, myWorld, 0, 2, 1) {}
Block::Block(int x, int y, StudentWorld* myWorld, bool isStar = false, bool isFlower = false, bool isShroom = false) :
	IdleActors(x, y, IID_BLOCK, myWorld), starBlock(isStar), flowerBlock(isFlower), shroomBlock(isShroom), beenBonked(false) {}
Pipe::Pipe(int x, int y, StudentWorld* myWorld) : IdleActors(x, y, IID_PIPE, myWorld) {}

void Block::bonk(Actor* bonker)
{ 
	if ((!starBlock && !flowerBlock && !shroomBlock) || beenBonked) //Play bonk sound if block has no goodie or goodie has already been bonked out
	{
		getWorld()->playSound(SOUND_PLAYER_BONK);
	}
	else if (!beenBonked) //Bonk out goodie otherwise
	{
		getWorld()->playSound(SOUND_POWERUP_APPEARS);
		beenBonked = true;
		
		if (starBlock)
			getWorld()->addActor(new Star((int)getX(), (int)getY() + 8, getWorld()));
		else if(flowerBlock)
			getWorld()->addActor(new Flower((int)getX(), (int)getY() + 8, getWorld()));
		else if (shroomBlock)
			getWorld()->addActor(new Mushroom((int)getX(), (int)getY() + 8, getWorld()));
	}
}



//FINISH LINE CLASSES
Finish::Finish(int x, int y, int IID, StudentWorld* myWorld) : Actor(IID, x, y, myWorld, 0, 1, 1) {}
Flag::Flag(int x, int y, StudentWorld* myWorld) : Finish(x, y, IID_FLAG, myWorld) {}
Mario::Mario(int x, int y, StudentWorld* myWorld) : Finish(x, y, IID_MARIO, myWorld) {}

void Finish::doSomething()
{
	if (!isAlive()) //Do nothing if object is dead
		return;

	if (getWorld()->overlaps(getX(), getY(), getWorld()->gamePeach())) //Finish level/game and increase score if peach overlaps
	{
		getWorld()->increaseScore(1000);
		setDead();
		complete();
	}
}


//GOODIE CLASSES
Goodie::Goodie(int x, int y, int IID, StudentWorld* myWorld): Actor(IID, x, y, myWorld, 0, 1, 1) {}
Flower::Flower(int x, int y, StudentWorld* myWorld) : Goodie(x, y, IID_FLOWER, myWorld) {}
Mushroom::Mushroom(int x, int y, StudentWorld* myWorld) : Goodie(x, y, IID_MUSHROOM, myWorld) {}
Star::Star(int x, int y, StudentWorld* myWorld) : Goodie(x, y, IID_STAR, myWorld) {}

void Goodie::doSomething()
{
	//Give Peach power and do necessary function if Peach touches goodie
	if (getWorld()->overlaps(getX(), getY(), getWorld()->gamePeach()))
	{
		getWorld()->increaseScore(pointsAdded());
		givePeachPower();
		getWorld()->gamePeach()->setHitpts(2);
		setDead();
		getWorld()->playSound(SOUND_PLAYER_POWERUP);
		return;
	}
	//Move 2 pixels to the side and down
	move(2, true);
}

//PROJECTILE CLASSES
Projectile::Projectile(int x, int y, int dir, int IID, StudentWorld* myWorld) : Actor(IID, x, y, myWorld, dir, 1, 1) {}
PiranhaFireball::PiranhaFireball(int x, int y, int dir, StudentWorld* myWorld): Projectile(x, y, dir, IID_PIRANHA_FIRE, myWorld) {}
PeachFireball::PeachFireball(int x, int y, int dir, StudentWorld* myWorld) : Projectile(x, y, dir, IID_PEACH_FIRE, myWorld) {}
Shell::Shell(int x, int y, int dir, StudentWorld* myWorld) : Projectile(x, y, dir, IID_SHELL, myWorld) {}

void Projectile::doSomething()
{
	Actor* overlap;
	//Damage overlaping object if it is damagable and fulfills criteria of object
	if (getWorld()->objectBlocking(getX(), getY(), this, overlap)  && conditionToDamage(overlap))
	{
		overlap->damage();
		setDead();
		return;
	}
	//Move 2 pixels to the side and down
	move(2, true);
}



//ENEMY CLASSES
Enemy::Enemy(int x, int y, int IID, int dir, StudentWorld* myWorld) : Actor(IID, x, y, myWorld, dir, 1, 0) {}
Goomba::Goomba(int x, int y, int dir, StudentWorld* myWorld) : Enemy(x, y, IID_GOOMBA, dir, myWorld) {}
Koopa::Koopa(int x, int y, int dir, StudentWorld* myWorld): Enemy(x, y, IID_KOOPA, dir, myWorld) {}
Piranha::Piranha(int x, int y, int dir, StudentWorld* myWorld) : Enemy(x, y, IID_PIRANHA, dir, myWorld), firingDelay(0) {}

void Enemy::doSomething()
{
	//Do nothing if Enemy is not alive
	if (!isAlive())
		return;

	//Bonk peach if enemy overlaps with object
	if (getWorld()->overlaps(getX(), getY(), getWorld()->gamePeach()))
	{
		getWorld()->gamePeach()->bonk(this);
		return;
	}

	//Switch direction if there is no supporting platform underneath
	Actor* placeholder;
	if (getDirection() == 0 && (!getWorld()->objectBlocking(getX() + SPRITE_WIDTH, getY() - 1, this, placeholder)  || !placeholder->canBlockActors()))
	{
		setDirection(180);
		return;
	}
	else if (getDirection() == 180 && (!getWorld()->objectBlocking(getX() - SPRITE_WIDTH, getY() - 1, this, placeholder) || !placeholder->canBlockActors()))
	{
		setDirection(0);
		return;
	}
		
		
	//Move 1 pixel sideways
	move(1, false);
}

void Enemy::endLife()
{
	getWorld()->increaseScore(100);
	setDead();
}

void Enemy::bonk(Actor* bonker)
{
	//Make sure that only Peach can bonk Enemy
	if (!bonker->isPeach())
		return;

	//Kill enemy and increase score if Peach is invincible
	if (getWorld()->gamePeach()->hasStarPower())
	{
		getWorld()->playSound(SOUND_PLAYER_KICK);
		endLife();
	}
}

void Enemy::damage()
{
	//Increase game score by 100 and set object dead
	endLife();
	if (isKoopa()) //Turn Koopa into Shell if necessary
		getWorld()->addActor(new Shell((int)getX(), (int)getY(), getDirection(), getWorld()));
}

void Piranha::doSomething()
{
	//Do nothing if Piranha is dead
	if (!isAlive())
		return;

	increaseAnimationNumber(); //Open and close mouth

	//Bonk peach if Piranha overlaps with Peach
	if (getWorld()->overlaps(getX(), getY(), getWorld()->gamePeach()))
	{
		getWorld()->gamePeach()->bonk(this);
		return;
	}

	//Check if Peach is on same level as Piranha, if not return
	if (!(getWorld()->gamePeach()->getY() < getY() + (1.5 * SPRITE_HEIGHT)
		&& getWorld()->gamePeach()->getY() > getY() - (1.5 * SPRITE_HEIGHT)))
		return;

	//Set direction to face Peach
	if (getWorld()->gamePeach()->getX() < getX())
		setDirection(180);
	else
		setDirection(0);

	//Decrement Firing delay if necessary
	if (firingDelay > 0)
	{
		firingDelay--;
		return;
	}

	//Check if Peach is within shooting distance and shoot if possible
	double distFromPeach = abs(getX() - getWorld()->gamePeach()->getX());
	if (distFromPeach < 8.0 * SPRITE_WIDTH)
	{
		getWorld()->addActor(new PiranhaFireball((int)getX(), (int)getY(), getDirection(), getWorld()));
		getWorld()->playSound(SOUND_PIRANHA_FIRE);
		firingDelay = 40;
	}
}