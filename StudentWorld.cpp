#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <iostream> 
#include <sstream> 
#include <iomanip>
using namespace std;


//Private Function that randomly returns one of two integers
int StudentWorld::random(int a, int b)
{
    srand(time(NULL));

    int r = rand() % 2;

    if (r == 0)
        return a;
    else
        return b;
}


GameWorld* createStudentWorld(string assetPath) 
{ 
    return new StudentWorld(assetPath); 
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), completedGame(false), completedLevel(false), worldPeach(nullptr){}


StudentWorld::~StudentWorld() 
{ 
    cleanUp();
}

//Adds new actor to container of actors
void StudentWorld::addActor(Actor* newActor)
{
    GameList.push_back(newActor);
}

//Checks if there is a object blocking the movement of the Actor "moving"
//Sets blocking to point to the actor blocking movement
//Returns BLOCKED_FROM_MOVING is actor in the way can block movement
//Returns MOVE_WITH_ACTOR_IN_WAY if there is an actor in the way that does not block movement
//Returns CAN_MOVE_FREELY if there is no actor in the way
bool StudentWorld::objectBlocking(double x, double y, Actor* moving, Actor*& blocking)
{
    bool moveWithActor = false;
    vector<Actor*>::iterator it = GameList.begin();
    while (it != GameList.end())
    {
        if ((*it) != moving && overlaps(x, y,(*it)))
        {
            blocking = (*it);

            if ((*it)->canBlockActors())
                return true;

            moveWithActor = true;
        }
        it++;
    }
    return moveWithActor;
}

//Returns true if the x and y coordinates overlap with another actor
bool StudentWorld::overlaps(double x, double y, Actor* stationary)
{
    //Dimensions of iterated item on grid
    double itstartX = stationary->getX() - SPRITE_WIDTH + 1;
    double itendX = stationary->getX() + SPRITE_WIDTH -1;
    double itstartY = stationary->getY() - SPRITE_HEIGHT + 1;
    double itendY = stationary->getY() + SPRITE_HEIGHT - 1;

    if (x <= itendX && x >= itstartX && y <= itendY && y >= itstartY)
        return true;

    return false;
}


int StudentWorld::init()
{
    //Code to load the current level into the file
    Level curr(assetPath());
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << getLevel() << ".txt";
    Level::LoadResult result = curr.loadLevel(oss.str());

    //Return error value if file is faulty/not found
    if (result == Level::load_fail_file_not_found || result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
 
    //Loads all actors into game
    Level::GridEntry get;
    for (int x = 0; x < VIEW_WIDTH/SPRITE_WIDTH; x++)
    {
        for (int y = 0; y < VIEW_HEIGHT/SPRITE_HEIGHT; y++)
        {
            get = curr.getContentsOf(x, y);
            switch (get)
            {
            case Level::peach:
                worldPeach = new Peach(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, this);
                addActor(worldPeach);
                break;
            case Level::block:
                addActor(new Block(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, this, false, false, false));
                break;
            case Level::flag:
                addActor(new Flag(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, this));
                break;
            case Level::pipe:
                addActor(new Pipe(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, this));
                break;
            case Level::mario:
                addActor(new Mario(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, this));
                break;
            case Level::koopa:
                addActor(new Koopa(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, random(0, 180), this));
                break;
            case Level::goomba:
                addActor(new Goomba(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, random(0, 180), this));
                break;
            case Level::piranha:
                addActor(new Piranha(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, random(0, 180), this));
                break;
            case Level::flower_goodie_block:
                addActor(new Block(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, this, false, true, false));
                break;
            case Level::mushroom_goodie_block:
                addActor(new Block(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, this, false, false, true));
                break;
            case Level::star_goodie_block:
                addActor(new Block(x * SPRITE_WIDTH, y * SPRITE_HEIGHT, this, true, false, true));
                break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    //Code to make all actors in game do something
    int size = GameList.size();
    for(int i = 0; i < size; i++)
    {
        if (GameList[i]->isAlive())
        {
            GameList[i]->doSomething();

            if (!worldPeach->isAlive()) //Check if Peach is dead
            {
                playSound(SOUND_PLAYER_DIE);
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            
            if (completedGame) //Check if Game is done
            {
                playSound(SOUND_GAME_OVER);
                return GWSTATUS_PLAYER_WON;
            }

            if (completedLevel) //Check if Level is completed
            {
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
   
    //Code to remove dead objects from game
    for (int i = 0; i < GameList.size(); i++)
    {
        if (!GameList[i]->isAlive())
        {
            delete GameList[i];
            GameList.erase(GameList.begin() + i);
            i--;
        }
    }

    //Code to update text displayed at top of screen
    ostringstream oss;
    oss << "Lives: " << getLives() << "  Level: " << getLevel() << "  Points: " << getScore();

    if (worldPeach->hasStarPower())
        oss << " StarPower!";
    
    if(worldPeach->hasShootPower())
        oss << " ShootPower!";

    if (worldPeach->hasJumpPower())
        oss << " JumpPower!";

    setGameStatText(oss.str());

    //Return value to continue game
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
    //Free all dynamically allocated actors
    std::vector<Actor*>::iterator it;
    it = GameList.begin();
    while (it != GameList.end())
    {
        delete (*it);
        it++;
    }
    //Clear the container of actors for reloading purposes
    GameList.clear();

    //Reset booolean value that specifies level completion
    completedLevel = false;
}
