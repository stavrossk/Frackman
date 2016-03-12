#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld * worldin, bool m_alive = true);

	Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);

	virtual ~Actor();

	virtual void annoy(unsigned int n);
	// If the actor can be annoyed, reduces their HP.

	virtual bool isProtesterPickup();
	// Returns true if actor can be picked up by a protester.

	virtual bool isPickup();
	// Returns true if actor can be picked up by the FrackMan.
	
	// isPickup and isProtesterPickup() were defined differently to facilitate the 
	// addition of future goodies that could be picked up by either type of actor.
	
	virtual bool isCollision();
	// Returns true if actor is a collision object.

	virtual bool isProtester();
	// Returns true if actor is a type of protester.

	StudentWorld * getWorld();
	// Returns a pointer to the StudentWorld the actors occupy.

	void kill();
	// Sets the actor's status as dead so that it can be removed from the oil field during the next tick.

	virtual void doSomething() = 0;
	// This function is redefined for each actor to include the actions that must be performed by that actor every tick.

	bool isAlive();

private:
	StudentWorld * world;
	bool m_alive;
};

class Boulder : public Actor
{
public:
	Boulder(int startX, int startY, StudentWorld * worldin);

	virtual void doSomething();

	virtual bool isCollision();

	virtual ~Boulder()
	{	}

private:
	int m_stability; 
	// m_stability == 2: boulder is stable
	// m_stability == 1: boulder is about to fall but is waiting
	// m_stability == 0: boulder is falling
	int m_tickCount;
};

class Goodie : public Actor
{
public:
	Goodie(int imageID, int startX, int startY, StudentWorld * worldin);
	
	virtual ~Goodie();

	virtual void doSomething() = 0;

	virtual void addInventory() = 0;
	// Adds the goodie to the FrackMan's inventory.

	virtual bool isPickup();
	// Returns true for most types of goodies, with the exception of those meant for protesters.

	void bePickedUp();
};

class HiddenGoodie : public Goodie 
{
public:
	HiddenGoodie(int imageID, int startX, int startY, StudentWorld * worldin);
	// Unlike other actors, a HiddenGoodie is initialized with setVisible(false).

	virtual void addInventory() = 0;

	virtual void doSomething();
	// Checks for the FrackMan within a radius of 4. Sets itself visible if FrackMan is found.

	virtual ~HiddenGoodie();
};

class TemporaryGoodie : public Goodie
{
public:
	TemporaryGoodie(int imageID, int startX, int startY, StudentWorld *worldin, int lifetime);

	virtual void addInventory() = 0;

	virtual void doSomething();
	// Decrements m_lifetime every tick. Kill()ed once lifetime is over.

	virtual ~TemporaryGoodie();

private:
	int m_lifetime;
};

class Sonar : public TemporaryGoodie
{
public:
	Sonar(int startX, int startY, StudentWorld * worldin, int lifetime);
	virtual ~Sonar();

	virtual void addInventory();
};

class BribeNugget : public TemporaryGoodie
{
public:
	BribeNugget(int startX, int startY, StudentWorld * worldin);

	virtual void addInventory();

	virtual bool isProtesterPickup();
	virtual bool isPickup();
	// A BribeNugget is meant for the protesters, not the FrackMan.

	virtual ~BribeNugget();
};

class Nugget : public HiddenGoodie
{
public:
	Nugget(int startX, int startY, StudentWorld * worldin);
	
	virtual ~Nugget();

	virtual void addInventory();
};

class Oil : public HiddenGoodie
{
public:
	Oil(int startX, int startY, StudentWorld * worldin);
	
	virtual ~Oil();

	virtual void addInventory();
};

class Pool : public TemporaryGoodie
{
public:
	Pool(int startX, int startY, StudentWorld * worldin, int lifetime);
	
	virtual ~Pool();

	virtual void addInventory();
};

class Squirt : public Actor
{
public:
	Squirt(int startX, int startY, Direction dir, StudentWorld * worldin);
	
	virtual ~Squirt();

	virtual void doSomething(); 

private:
	int m_lifetime;
};

class HPActor : public Actor // A moving actor with hit points
{
public:
	HPActor(int imageID, int startX, int startY, Direction dir, int HP, StudentWorld * worldin);
	
	virtual ~HPActor();

	int getHP() const;
	// Returns the HP of an actor.
	
	virtual void annoy(unsigned int n);	
	// Subtracts n from HP. 
	
	virtual void beAnnoyed() = 0;	// Triggered when an HPActor is annoyed (e.g. when hit by falling boulder or squirt).
	virtual void deathThroes() = 0; // Triggered when an HPActor is killed. 

private:
	int m_HP;
};


class Protester : public HPActor
{
public:
	Protester(int startX, int startY, int ticksBetweenMoves, StudentWorld* worldin, int HP = 5, int imageID = IID_PROTESTER);

	virtual ~Protester();

	virtual bool isProtester();
	virtual void beAnnoyed();
	virtual void deathThroes();
	virtual void doSomething();
	
	void yell();
	// Causes the protester to yell at the FrackMan, annoying him and playing a yell sound.

	void restFor(int n);
	// Causes a tick delay n until the next action.

protected:
	virtual void beBribed();
	// Causes a normal protester to pick up a BribeNugget and subsequently toggle m_leaveField to true.

	virtual bool findFrackMan();
	// Returns false for regular protesters.

	void moveInThisDirection(Direction dir);

private:
	int m_ticksBetweenMoves;
	bool m_leaveField;
	int m_numMovesCurrentDirection; // number of squares to move in current direction
	int m_restingTicks;
	int m_shoutTicks;
	int m_perpendicularTicks;

	bool checkVerticalLineofSight(int frackX, int frackY); 
	bool checkHorizontalLineofSight(int frackX, int frackY);
	// These functions check if the FrackMan is in line of sight of a Protester.
	// Causes the Protester to walk in the FrackMan's direction and returns true if so.

	virtual void lookForBribe();
	// Causes the protester to search its radius for a BribeNugget. Picks it up and returns true if found.
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(int startX, int startY, int ticksBetweenMoves, StudentWorld * worldin);

	virtual ~HardcoreProtester();

	virtual void beBribed();
	// A hardcore protester only rests for a certain amount of time upon being bribed.

	virtual bool findFrackMan();
	// Returns true if the FrackMan is within a radius of m_huntRadius, causes the 
	// HardcoreProtester to move towards the FrackMan.

private:
	int m_huntRadius; // Number of moves away a HardcoreProtester is allowed to findFrackMan().
};


class FrackMan : public HPActor 
{
public:
	FrackMan(StudentWorld * worldin);

	virtual ~FrackMan();

	// Acessors
	int getWater() const;
	int getSonar() const;
	int getGold() const;

	// Mutators
	void addWater();
	void addSonar();
	void addGold();

	virtual void beAnnoyed();
	virtual void deathThroes();
	virtual void doSomething();

private: 
	void squirtWater();
	void dropBribe();
	void useSonar();
	int m_water;
	int m_sonars;
	int m_nuggets;
};

class Dirt : public Actor
{
public:
	Dirt(int startX, int startY);

	virtual ~Dirt();

	virtual void doSomething();
};


#endif // ACTOR_H_
