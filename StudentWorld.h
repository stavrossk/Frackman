#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <math.h>
#include <random>
#include <queue>
using namespace std;

const int MAX_ROWS = 64;
const int MAX_COLS = 64;
const int BLOCKED = MAX_ROWS * MAX_COLS;
const int UNEXPLORED = 0;

struct Coord
{
public:
	Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
	int r() const { return m_r; }
	int c() const { return m_c; }
private:
	int m_r;
	int m_c;
};

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{	for (int x = 0; x < MAX_COLS; x++)
			for (int y = 0; y < MAX_ROWS; y++)
				dirtMap[y][x] = nullptr;
		m_tickCount = 0;
	}

	virtual ~StudentWorld();

	virtual int init();
	int move();
	virtual void cleanUp();

	void addWater();
	void addSonar();
	void addGold();
	void addOil();
	// These functions add particular types of items to the FrackMan's inventory.

	void addActor(Actor * a);
	// Adds an actor to the oilfield.

	void annoyFrackman(int a);
	// Annoys the FrackMan by an amount a.

	void revealNearbyObjects(Actor * act, int r);
	// Reveals all hidden goodies in a radius r.

	int frackX();
	int frackY();
	// These functions return the (x,y) position of FrackMan.

	Actor * inRadius(int x, int y, int r, bool frack, bool protester, bool goodie, bool collision);
	// Returns a pointer to types of actors that are in a radius r of the actor.
	// Using the last four parameters, you can specify to this function whether you are 
	// looking for a frackman, a protester, a goodie, a collision or some combination of the four.
	
	bool checkRadius(int x, int y, int r, bool frack, bool protester, bool goodie, bool collision);
	// Simple function that calls inRadius and returns only a true/false depending on whether anything was found.

	bool isClear(int x, int y, int n); 
	// Checks if an n x n square is occupied by either dirt or boulders.
	
	bool isDirt(int x, int y);
	// Checks if (x,y) is occupied by dirt, or is out of bounds.

	bool isCollision(int x, int y); 
	// Simple function that uses checkRadius to determine if (x,y) is occupied by a collision object.

	void updatePathMap(int pathMap[][MAX_COLS], int tarX, int tarY);
	// This function updates a pathMap using a BFS.

	GraphObject::Direction nextDirectionToTake(Actor * protester, bool leavingOilField);
	// Uses the pathMaps to determine the next direction a fully annoyed protester or a hunting Hardcore Protester should move.

	bool StudentWorld::isThisDirectionClear(int x, int y, GraphObject::Direction dir);
	// Returns true if the four blocks facing the argument direction (relative to (x,y)) are clear of boulders or dirt.

private:
	vector <Actor*> actorList;
	FrackMan * Fracker;

	Dirt * dirtMap		[MAX_ROWS][MAX_COLS];
	int pathMapExit		[MAX_ROWS][MAX_COLS];
	int pathMapFrack	[MAX_ROWS][MAX_COLS];

	int m_numBoulders;
	int m_numNuggets;
	int m_numOil;

	int m_tickCount; 
	int m_protesterTicks; // Tick delay between protester spawns
	int m_goodieTicks; // Tick lifetime of any temporary goodie
	int m_protesterTickDelay; // Tick delay between protester actions
	int m_maxProtesters; 
	int m_numProtesters;
	int m_probOfHardcore; // Probabibility of a hardcore protester spawning during a protester spawn
	double m_probOfGoodie; // Probability of a goodie spawning during a tick 

	int randInt(int min, int max); // returns a random number from min to max
	void setText();
	bool clearDirt(int x, int y);
		// Clear a 4x4 block of dirt whose lower left corner is (x,y). Returns true if at least one dirt block was cleared.

	int mini(const int &a, const int &b)
	{
		return (a>b) ? b : a;
	}
	int maxi(const int &a, const int &b)
	{
		return (a>b) ? a : b;
	}
};

#endif // STUDENTWORLD_H_
