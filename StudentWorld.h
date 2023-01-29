#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>

class Actor;
class Peach;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	~StudentWorld();

	//VIRTUAL FUNCTION DECLARATIONS
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	//NON-VIRTUAL FUNCTION DECLARATIONS
	void addActor(Actor* newActor);
	void setLevelComplete() { completedLevel = true; }
	void setGameComplete() { completedGame = true; }
	bool overlaps(double x, double y, Actor* stationary);
	bool objectBlocking(double x, double y, Actor* moving, Actor*& blocking);

	//ACCESSOR TO PEACH
	Peach* gamePeach() { return worldPeach; }
private:
	int random(int a, int b);
	std::vector<Actor*> GameList;
	Peach* worldPeach;
	bool completedLevel, completedGame;
};

#endif // STUDENTWORLD_H_