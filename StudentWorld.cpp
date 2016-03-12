#include "StudentWorld.h"

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld()
{
	for (int x = 0; x < MAX_COLS; x++)
		for (int y = 0; y < 59; y++)
			if (dirtMap[y][x] != nullptr)
			{	delete dirtMap[y][x];
				dirtMap[y][x] = nullptr;
			}
	
	delete Fracker;

	for (int i = 0; actorList.size() > 0;)
	{	delete actorList[i];
		actorList.erase(actorList.begin());
	}
}

bool StudentWorld::isThisDirectionClear(int x, int y, GraphObject::Direction dir)
{

	if (dir == GraphObject::up)
	{
		for (int i = x; i < x + 4; i++)
			if (y + 4 >= 64 || isDirt(i, y + 4)) return false;
		if (isCollision(x, y + 1)) return false;
	}
	if (dir == GraphObject::down)
	{
		for (int i = x; i < x + 4; i++)
			if (y - 1 < 0 || isDirt(i, y - 1)) return false;
		if (isCollision(x, y - 4 /* Needs to be 4, otherwise boulder will detect itself while falling. */)) return false;
	}
	if (dir == GraphObject::right)
	{
		for (int i = y; i < y + 4; i++)
			if (x + 4 >= 64 || isDirt(x + 4, i)) return false;
		if (isCollision(x + 1, y)) return false;
	}
	if (dir == GraphObject::left)
	{
		for (int i = y; i < y + 4; i++)
			if (x - 1 < 0 || isDirt(x - 1, i)) return false;
		if (isCollision(x - 1, y)) return false;
	}
	return true;
}

void StudentWorld::updatePathMap(int pathMap[][MAX_COLS], int tarX, int tarY)
{
	// Start by reseting each square in the heat maps that are not BLOCKED (by dirt or collision objects).
	for (int x = 0; x < MAX_COLS; x++)
		for (int y = 0; y < MAX_ROWS; y++)
			if (pathMap[y][x] != BLOCKED) pathMap[y][x] = UNEXPLORED;

	// Then make any square left/down-adjacent to a BLOCKED square within three squares very large numbers.
	// This ensures that protesters do not attempt to overlap their 4x4 image with BLOCKED squares.
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 64; y++)
			if (pathMap[y][x] != BLOCKED)
				for (int i = x; i < x + 4; i++)
					for (int j = y; j < y + 4; j++)
						if (pathMap[j][i] == BLOCKED)
							pathMap[y][x] = 3600;

	Coord cur(0, 0);
	queue<Coord> path;
	Coord start(tarY, tarX);
	path.push(start);
	int distance = 0;
	int y, x;

	while (!path.empty())
	{
		distance++;
		cur = path.front();
		path.pop();
		x = cur.c();
		y = cur.r();

		if (y - 1 >= 0 && pathMap[y - 1][x] == UNEXPLORED)	// check SOUTH
		{
			path.push(Coord(y - 1, x));
			pathMap[y - 1][x] = distance;
		}
		if (x + 1 < 64 && pathMap[y][x + 1] == UNEXPLORED)	// check EAST
		{
			path.push(Coord(y, x + 1));
			pathMap[y][x + 1] = distance;
		}
		if (y + 1 < 64 && pathMap[y + 1][x] == UNEXPLORED)	// check NORTH
		{
			path.push(Coord(y + 1, x));
			pathMap[y + 1][x] = distance;
		}
		if (x - 1 >= 0 && pathMap[y][x - 1] == UNEXPLORED)	// check WEST
		{
			path.push(Coord(y, x - 1));
			pathMap[y][x - 1] = distance;
		}
	}

	return; // Should only reach here once every path has been explored.	
}

GraphObject::Direction StudentWorld::nextDirectionToTake(Actor * protester, bool leavingOilField)
{
	int x = protester->getX();
	int y = protester->getY();
	//int cur = pathMapExit[y][x];
	int cur = BLOCKED;
	GraphObject::Direction curDir = GraphObject::none;

	if (leavingOilField) // Walk to exit
	{
		if (x >= 59 && y >= 59) return GraphObject::right;
		if (pathMapExit[y + 1][x] < cur && pathMapExit[y + 1][x] != 3600 /*&& isThisDirectionClear(x, y, GraphObject::up)*/) {
			curDir = GraphObject::up;
			cur = pathMapExit[y + 1][x];
		}
		if (pathMapExit[y - 1][x] < cur && pathMapExit[y - 1][x] != 3600/* && isThisDirectionClear(x, y, GraphObject::down)*/) {
			curDir = GraphObject::down;
			cur = pathMapExit[y - 1][x];
		}
		if (pathMapExit[y][x + 1] < cur && pathMapExit[y][x + 1] != 3600/* && isThisDirectionClear(x, y, GraphObject::right)*/) {
			curDir = GraphObject::right;
			cur = pathMapExit[y][x + 1];
		}
		if (pathMapExit[y][x - 1] < cur && pathMapExit[y][x - 1] != 3600/*&& isThisDirectionClear(x, y, GraphObject::left)*/) {
			curDir = GraphObject::left;
			cur = pathMapExit[y][x - 1];
		}
	}
	if (!leavingOilField) // Walk to FrackMan
	{
	
		if (pathMapFrack[y + 1][x] < cur && pathMapFrack[y + 1][x] != 3600 /*&& isThisDirectionClear(x, y, GraphObject::up)*/) {
			curDir = GraphObject::up;
			cur = pathMapFrack[y + 1][x];
		}
		if (pathMapFrack[y - 1][x] < cur && pathMapFrack[y - 1][x] != 3600/* && isThisDirectionClear(x, y, GraphObject::down)*/) {
			curDir = GraphObject::down;
			cur = pathMapFrack[y - 1][x];
		}
		if (pathMapFrack[y][x + 1] < cur && pathMapFrack[y][x + 1] != 3600/* && isThisDirectionClear(x, y, GraphObject::right)*/) {
			curDir = GraphObject::right;
			cur = pathMapFrack[y][x + 1];
		}
		if (pathMapFrack[y][x - 1] < cur && pathMapFrack[y][x - 1] != 3600/*&& isThisDirectionClear(x, y, GraphObject::left)*/) {
			curDir = GraphObject::left;
			cur = pathMapFrack[y][x - 1];
		}
	}
	return curDir; 
}


int StudentWorld::init()
{
	for (int x = 0; x < MAX_COLS; x++)
		for (int y = 0; y < MAX_ROWS; y++)
		{
			pathMapFrack[y][x] = 0;
			pathMapExit[y][x] = 0;
			dirtMap[y][x] = nullptr;
		}

	for (int x = 0; x < MAX_COLS; x++)
		for (int y = 0; y <= 59; y++)
			if (!(x >= 30 && x <= 33 && y >= 4 && y <= 59))
			{
				dirtMap[y][x] = new Dirt(x, y);
				pathMapFrack[y][x] = BLOCKED;
				pathMapExit[y][x] = BLOCKED;
			}

	Fracker = new FrackMan(this);

	// Initialization of member variables associated with tick counts and actor spawning
	m_tickCount = 0;
	m_numProtesters = 0;
	m_maxProtesters = mini(15, 2 + getLevel()*1.5);
	m_protesterTicks = maxi(25, 200 - getLevel());
	m_goodieTicks = mini(15, 2 + getLevel());
	m_probOfHardcore = mini(90, getLevel() * 10 + 30);
	m_probOfGoodie = (1.0 / ( getLevel() * 25.0 + 300.0)) * 1000000 ;
	m_protesterTickDelay = maxi(0, 3 - getLevel() / 4);
	m_tickCount = m_protesterTicks - 1;

	m_numBoulders = mini((getLevel() / 2 + 2), 6);
	m_numNuggets = maxi(5 - getLevel() / 2, 2);
	m_numOil = mini(2 + getLevel(), 20);

	// These loops place boulders/gold/oil throughout the field at the beginning of a level.
	for (int i = 0; i < m_numBoulders; i++)  // Boulder Spawner
	{
		int x, y;
		do{
			x = rand() % 59;
			y = rand() % 35 + 20;
		} while (checkRadius(x, y, 6, false, false, true, true) || (x >= 27 && x <= 33 && y >= 4 && y <= 59));
		actorList.push_back(new Boulder(x, y, this));
	
		for (int j = x; j < x + 4; j++)
			for (int k = y; k < y + 4; k++)
			{
				pathMapFrack[k][j] = BLOCKED;
				pathMapExit[k][j] = BLOCKED;
			}
		clearDirt(x, y);
	}
	for (int i = 0; i < m_numNuggets; i++)	// Gold Spawner
	{
		int x, y;
		do{
			x = rand() % 59;
			y = rand() % 35 + 20;
		} while (checkRadius(x, y, 6, false, false, true, true) || (x >= 27 && x <= 33 && y >= 4 && y <= 59));
		actorList.push_back(new Nugget(x, y, this));
	}
	for (int i = 0; i < m_numOil; i++)		// Oil Spawner
	{
		int x, y;
		do{
			x = rand() % 59;
			y = rand() % 35 + 20;
		} while (checkRadius(x, y, 6, false, false, true, true) || (x >= 27 && x <= 33 && y >= 4 && y <= 59));
		actorList.push_back(new Oil(x, y, this));
	}

	updatePathMap(pathMapExit, 60, 60); // Update pathMapExit
	updatePathMap(pathMapFrack, Fracker->getX(), Fracker->getY());  // Update pathMapFrack
	return GWSTATUS_CONTINUE_GAME;
}
int StudentWorld::move()
{
	setText();
	m_tickCount++;

	if (m_numOil <= 0)
		return GWSTATUS_FINISHED_LEVEL;

	// Chance of spawning new Protesters every tick.
	if (m_tickCount >= m_protesterTicks && m_numProtesters < m_maxProtesters) 
	{
		m_numProtesters++;
		m_tickCount = 0;
		if ((rand() % 100) < m_probOfHardcore)
			actorList.push_back(new HardcoreProtester(60, 60, m_protesterTickDelay, this));
		else
			actorList.push_back(new Protester(60, 60, m_protesterTickDelay, this));
	}

	// Chance of spawning new Sonars or Water Pools every tick.
	if (randInt(1, 1000000) < m_probOfGoodie)
		if (rand() % 99 < 20) actorList.push_back(new Sonar(0, 60, this, maxi(25, 200 - getLevel())));
		else
		{	int x, y;
			do
			{	x = randInt(0, 64);
				y = randInt(0, 64);
			} while (!isClear(x, y, 4));
			actorList.push_back(new Pool(x, y, this, maxi(25, 200 - getLevel())));
		}

	// Ends level if FrackMan is dead.
	if (!Fracker->isAlive())
	{	decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	// Checks which actors have died during the current tick and deletes them.
	for (int i = 0; i < actorList.size();)
	{	
		if (!actorList[i]->isAlive())
		{
			if (actorList[i]->isProtester()) m_numProtesters--;
			if (actorList[i]->isCollision()) updatePathMap(pathMapExit, 60, 60); // Update pathMapExit if a collision object disappears.
			delete actorList[i];
			actorList.erase(actorList.begin() + i);
		}
		else
		{	
			actorList[i]->doSomething();
			i++;
		}
	}

	// Asks the FrackMan to do something. 
	// If there is dirt in its new location, clears it and makes a digging sound.
	// If dirt is cleared, or if the FrackMan moves, must also update pathfinding maps for protesters.
	int tempX = Fracker->getX();
	int tempY = Fracker->getY();
	Fracker->doSomething();
	if (clearDirt(Fracker->getX(), Fracker->getY()))
	{
		updatePathMap(pathMapExit, 60, 60); // If dirt is cleared, update pathMapsExit.
		playSound(SOUND_DIG);
	}
	if (tempX != Fracker->getX() || tempY != Fracker->getY()) updatePathMap(pathMapFrack, Fracker->getX(), Fracker->getY()); // If the FrackMan has moved, update pathMapFrack.

	return GWSTATUS_CONTINUE_GAME;
}
void StudentWorld::cleanUp()
{
	for (int x = 0; x < MAX_COLS; x++)
		for (int y = 0; y <= 59; y++)
			if (dirtMap[y][x] != nullptr)
			{	delete dirtMap[y][x];
				dirtMap[y][x] = nullptr; 
				// A pointer in dirtMap is set to nullptr to signify that there is no dirt at that location.
			}

	for (int i = 0; actorList.size() > 0; )
	{	delete actorList[i];
		actorList.erase(actorList.begin());	}

	delete Fracker;
}

void StudentWorld::setText()
{
	int score = getScore();
	string gameText = "Scr: ";
	if (score > 999999) score = 999999;
	else if (score < 10) gameText += "00000";
	else if (score < 100) gameText += "0000";
	else if (score < 1000) gameText += "000";
	else if (score < 10000) gameText += "00";
	else if (score < 100000) gameText += "0";
	gameText += to_string(getScore());

	gameText += "  Lvl: ";
	int level = getLevel();
	if (level > 99) level = 99;
	else if (level < 10) gameText += " ";
	gameText += to_string(level);

	gameText += "  Lives: ";
	gameText += to_string(getLives());

	gameText += "  Hlth: ";
	int health = Fracker->getHP();
	if (health < 10) gameText += " ";
	gameText += to_string(health);

	gameText += "0%  Wtr: ";
	int water = Fracker->getWater();
	if (water > 99) water = 99;
	else if (water < 10) gameText += " ";
	gameText += to_string(water);

	gameText += "  Gld: ";
	int gold = Fracker->getGold();
	if (gold > 99) gold = 99;
	else if (gold < 10) gameText += " ";
	gameText += to_string(gold);
	
	gameText += "  Sonar: ";
	int sonar = Fracker->getSonar();
	if (sonar > 99) sonar = 99;
	else if (sonar < 10) gameText += " ";
	gameText += to_string(sonar);

	gameText += "  Oil Left: ";
	if (m_numOil < 10) gameText += " ";
	gameText += to_string(m_numOil);

	setGameStatText(gameText);
}

void StudentWorld::revealNearbyObjects(Actor * act, int r)
{
	for (int i = 0; i < actorList.size(); i++)
	{
		// Checks if actor is within a radius of 12 to frackman.
		if (checkRadius(actorList[i]->getX(), actorList[i]->getY(), 12, true, false, false, false)) 
			actorList[i]->setVisible(true);
	}
}

void StudentWorld::addActor(Actor * a) 
{
	actorList.push_back(a);
}

bool StudentWorld::clearDirt(int x, int y)
{
	{
		bool wasdirtcleared = false;
		for (int i = x; i < x + 4; i++)
			for (int j = y; j < y + 4; j++)
				if (dirtMap[j][i] != nullptr)
				{
					delete dirtMap[j][i];
					dirtMap[j][i] = nullptr;
					pathMapExit[j][i] = UNEXPLORED;
					pathMapFrack[j][i] = UNEXPLORED;
					wasdirtcleared = true;
				}
		return wasdirtcleared;
	}
}

bool StudentWorld::isClear(int x, int y, int n)
{
	if (x < 0 || y < 0 || y >= MAX_ROWS || x >= MAX_COLS) return false;
	for (int i = x; i < n + x; i++)
		for (int j = y; j < n + y; j++)
		{
			if (isDirt(i, j)) return false;
		}
	if (isCollision(x, y)) return false;
	return true;
}


bool StudentWorld::isDirt(int x, int y)
{
	if (x < 0 || y < 0 || x > MAX_ROWS || y > MAX_COLS) return true;
	if (dirtMap[y][x] != nullptr) return true;
	return false;
}

bool StudentWorld::isCollision(int x, int y)
{
	return checkRadius(x, y, 3, false, false, false, true);
}

Actor * StudentWorld::inRadius(int x, int y, int r, bool frack, bool protester, bool goodie, bool collision)
{
	for (int i = x - r; i <= x + r; i++)
		for (int j = y - r; j <= y + r; j++)
		{
			if (i < 0 || j < 0) continue; // avoid accessing out of bounds 
			if (sqrt((double)(x - i)*(x - i) + (y - j)*(y - j)) > r) continue; // If outside radius do not check
			if (frack)
			{
				if (Fracker->getX() == i && Fracker->getY() == j)
					return Fracker;
			}
			if (protester)
			{
				for (int k = 0; k < actorList.size(); k++)
					if (actorList[k]->isProtester() && actorList[k]->getX() == i && actorList[k]->getY() == j)
						return actorList[k];
			}
			if (goodie)
			{
				for (int k = 0; k < actorList.size(); k++)
					if ((actorList[k]->isPickup() || actorList[k]->isProtesterPickup()) && actorList[k]->getX() == i && actorList[k]->getY() == j)
						return actorList[k];
			}
			if (collision)
			{
				for (int k = 0; k < actorList.size(); k++)
					if (actorList[k]->isCollision() && actorList[k]->getX() == i && actorList[k]->getY() == j)
						return actorList[k];
			}
		}
	return nullptr;
}

bool StudentWorld::checkRadius(int x, int y, int r, bool frack, bool protester, bool goodie, bool collision)
{
	return (inRadius(x, y, r, frack, protester, goodie, collision) == nullptr) ? false : true;
}

int StudentWorld::randInt(int min, int max)
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

void StudentWorld::addWater()
{	Fracker->addWater(); }
void StudentWorld::addSonar()
{	Fracker->addSonar(); }
void StudentWorld::addGold()
{	Fracker->addGold(); }
void StudentWorld::addOil()
{	m_numOil--;	}
int StudentWorld::frackX()
{	return Fracker->getX();	}
int StudentWorld::frackY()
{	return Fracker->getY();	}
void StudentWorld::annoyFrackman(int a)
{	Fracker->annoy(a);	}


// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

