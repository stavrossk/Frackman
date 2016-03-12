#include "Actor.h"
#include "StudentWorld.h"

////////////////////////////////////////////////////////////////////
/////////////////////// Actor Implementation ///////////////////////
////////////////////////////////////////////////////////////////////

Actor::Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld * worldin, bool m_alive)
	:GraphObject(imageID, startX, startY, dir, size, depth)
{	world = worldin;	}
Actor::Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth)
{	world = nullptr;	}

Actor::~Actor()
{	}

void Actor::annoy(unsigned int n)
{	return;			}

bool Actor::isProtesterPickup()
{	return false;	}

bool Actor::isPickup()
{	return false;	}

bool Actor::isCollision()
{	return false;	}

bool Actor::isAlive()
{	return m_alive;	}

StudentWorld * Actor::getWorld()
{	return world;	}

void Actor::kill()
{	m_alive = false;
	setVisible(false); }
	
bool Actor::isProtester()
{	return false;	}


////////////////////////////////////////////////////////////////////
/////////////////////// Boulder Implementation /////////////////////
////////////////////////////////////////////////////////////////////

Boulder::Boulder(int startX, int startY, StudentWorld * worldin)
	:Actor(IID_BOULDER, startX, startY, down, 1.0, 1, worldin)
{
	m_stability = 2;
	setVisible(true);
	m_tickCount = 0;
}

bool Boulder::isCollision()
{	return true;	}

void Boulder::doSomething()
{
	if (!isAlive()) return;
	if (getWorld()->isThisDirectionClear(getX(), getY(), down) && m_stability == 2) m_stability--; 
	if (m_stability == 1)
	{
		if (m_tickCount > 30)
		{
			getWorld()->playSound(SOUND_FALLING_ROCK);
			m_stability--;
		}
		else m_tickCount++;
	}
	if (m_stability == 0)
	{
		if (getY()-1 >= 0 && getWorld()->isThisDirectionClear(getX(), getY(), down))		// again need to check for dirt or other boulders
		{
			moveTo(getX(), getY() - 1);
			Actor * doomedfellow = getWorld()->inRadius(getX(), getY(), 3, true, true, false, false);
			if(doomedfellow != nullptr) doomedfellow->annoy(10); 
		}
		else kill();
	}
	return;
}

////////////////////////////////////////////////////////////////////
////////////////////// Squirt Implementation ///////////////////////
////////////////////////////////////////////////////////////////////

Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld * worldin)
	:Actor(IID_WATER_SPURT, startX, startY, dir, 1.0, 1.0, worldin)
{
	setVisible(true);
	m_lifetime = 0;
}

Squirt::~Squirt()
{	}

void Squirt::doSomething()
{
	Direction dir = getDirection();
	
	Actor * protester = getWorld()->inRadius(getX(), getY(), 3, false, true, false, false);
	if (protester != nullptr)
	{
		protester->annoy(2);
		kill();
		return;
	}

	m_lifetime++;
	if (m_lifetime >= 4)
	{
		kill();
		return;
	}

	if (dir == up)
	{	if (getWorld()->isDirt(getX(), getY() + 4) || getWorld()->isCollision(getX(), getY() + 4 ))
		{	kill();
			return;	}
		moveTo(getX(), getY() + 1);	}
	if (dir == down)
	{	if (getWorld()->isDirt(getX(), getY() - 1) || getWorld()->isCollision(getX(), getY() - 1))
		{	kill();
			return;	}
		moveTo(getX(), getY() - 1);	}
	if (dir == right)
	{	if (getWorld()->isDirt(getX() + 4, getY()) || getWorld()->isCollision(getX() + 4, getY()))
		{	kill();
			return;	}
		moveTo(getX() + 1, getY());	}		
	if (dir == left)
	{	if (getWorld()->isDirt(getX() - 1, getY()) || getWorld()->isCollision(getX() - 1, getY()))
		{	kill();
			return;	}
		moveTo(getX() - 1, getY());	}
	return;
}

////////////////////////////////////////////////////////////////////
////////////////////// Goodie Implementations //////////////////////
////////////////////////////////////////////////////////////////////

Goodie::Goodie(int imageID, int startX, int startY, StudentWorld * worldin)
	:Actor(imageID, startX, startY, right, 1.0, 2, worldin)
{	}

Goodie::~Goodie()
{	}

//inline void Goodie::doSomething()
//{	return;	}
//inline void Goodie::addInventory()
//{	return;	}
inline bool Goodie::isPickup()
{	return true;	}

void Goodie::bePickedUp()
{
	if (isPickup() && getWorld()->checkRadius(getX(), getY(), 3, true, false, false, false))
	{
		kill();
		addInventory();
		return;
	}
}

HiddenGoodie::HiddenGoodie(int imageID, int startX, int startY, StudentWorld * worldin)
	:Goodie(imageID, startX, startY, worldin)
{
	setVisible(false);
}

HiddenGoodie::~HiddenGoodie()
{	}

void HiddenGoodie::doSomething()
{
	if (!isAlive()) return;
	if (getWorld()->checkRadius(getX(), getY(), 4, true, false, false, false) && !isVisible())
	{
		setVisible(true);
		return;
	}
	else bePickedUp();
}

TemporaryGoodie::TemporaryGoodie(int imageID, int startX, int startY, StudentWorld *worldin, int lifetime)
	:Goodie(imageID, startX, startY, worldin)
{
	setVisible(true);
	m_lifetime = lifetime;
}

TemporaryGoodie::~TemporaryGoodie()
{	}

void TemporaryGoodie::doSomething()
{
	if (!isAlive()) return;

	m_lifetime--;
	if (m_lifetime <= 0) kill(); 
	// A temporary goodie needs to disappear once it has run its course.

	bePickedUp();
}

BribeNugget::BribeNugget(int startX, int startY, StudentWorld * worldin)
	:TemporaryGoodie(IID_GOLD, startX, startY, worldin, 100)
{	}

BribeNugget::~BribeNugget()
{	}

void BribeNugget::addInventory()
{
	return; 
}

inline bool BribeNugget::isProtesterPickup()
{
	return true;
}

inline bool BribeNugget::isPickup()
{
	return false;
}

Nugget::Nugget(int startX, int startY, StudentWorld * worldin)
	:HiddenGoodie(IID_GOLD, startX, startY, worldin)
{	}

Nugget::~Nugget()
{	}

Sonar::Sonar(int startX, int startY, StudentWorld * worldin, int lifetime)
	:TemporaryGoodie(IID_SONAR, startX, startY, worldin, lifetime)
{	}

Sonar::~Sonar()
{	}

Oil::Oil(int startX, int startY, StudentWorld * worldin)
	:HiddenGoodie(IID_BARREL, startX, startY, worldin)
{	}

Oil::~Oil()
{	}

Pool::Pool(int startX, int startY, StudentWorld * worldin, int lifetime)
	:TemporaryGoodie(IID_WATER_POOL, startX, startY, worldin, lifetime)
{	}

Pool::~Pool()
{	}

////////////////// addInventory() Functions ///////////////////////

void Sonar::addInventory()
{
	getWorld()->addSonar();
	getWorld()->increaseScore(75);
	getWorld()->playSound(SOUND_GOT_GOODIE);
}
void Nugget::addInventory()
{
	getWorld()->addGold();
	getWorld()->increaseScore(10);
	getWorld()->playSound(SOUND_GOT_GOODIE);

}
void Pool::addInventory()
{
	getWorld()->addWater();
	getWorld()->increaseScore(100);
	getWorld()->playSound(SOUND_GOT_GOODIE);

}
void Oil::addInventory()
{
	getWorld()->addOil();
	getWorld()->increaseScore(1000);
	getWorld()->playSound(SOUND_FOUND_OIL);

}

////////////////////////////////////////////////////////////////////
////////////////// HPActor Implementation //////////////////////////
////////////////////////////////////////////////////////////////////

HPActor::HPActor(int imageID, int startX, int startY, Direction dir, int HP, StudentWorld * worldin)
	:Actor(imageID, startX, startY, dir, 1.0, 0, worldin)
{
	m_HP = HP;
	setVisible(true);
}

HPActor::~HPActor()
{	}

inline int HPActor::getHP() const
{
	return m_HP;
}

void HPActor::annoy(unsigned int n)	// Subtracts n from HP. Returns false if HP falls to <= 0.
{
	m_HP -= n;
	if (m_HP <= 0)
	{
		deathThroes();
		return;
	}
	beAnnoyed();
}

////////////////////////////////////////////////////////////////////
////////////////// Protester Implementation ////////////////////////
////////////////////////////////////////////////////////////////////

inline bool Protester::isProtester()
{	return true;	}

inline void Protester::restFor(int n)
{	m_restingTicks = n;	}


void Protester::yell()
{
		getWorld()->playSound(SOUND_PROTESTER_YELL);
		getWorld()->annoyFrackman(2);
		m_shoutTicks = 0;
}

inline void Protester::deathThroes()
{
	getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
	m_restingTicks = 0; // This is so that there is no annoy-delay after protester is fully annoyed.
}

void Protester::beAnnoyed()
{
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	int n = (100 - getWorld()->getLevel() * 10);
	if (50 > n) n = 50;
	restFor(n);
}

bool Protester::checkVerticalLineofSight(int frackX, int frackY) // check vertical line of sight for FrackMan
{
	bool los;
	int x = getX();
	int y = getY();
	if (frackX == x) 
	{
		los = true;
		for (int i = y; (y < frackY) ? i < frackY : frackY < i; (y < frackY) ? i++ : i--)
			if (!getWorld()->isThisDirectionClear(x, y, (y < frackY) ? up : down))
			{
				los = false;
				break;
			}
		if (los && getY() < frackY)
		{
			setDirection(up);
			moveTo(getX(), getY() + 1);
			m_numMovesCurrentDirection = 0;
			return true;
		}
		else if (los)
		{
			setDirection(down);
			moveTo(getX(), getY() - 1);
			m_numMovesCurrentDirection = 0;
			return true;
		}
	}
	return false;
}
bool Protester::checkHorizontalLineofSight(int frackX, int frackY) // check horizontal line of sight for FrackMan
{
	bool los;
	int x = getX();
	int y = getY();
	if (frackY == y) 
	{
		los = true;
		for (int i = x; (x < frackX) ? i < frackX : frackX < i; (x < frackX) ? i++ : i--)
			if (!getWorld()->isThisDirectionClear(x, y, (x < frackY) ? right : left))
			{
				los = false;
				break;
			}
	
		if (los && getX() < frackX)
		{
			setDirection(right);
			moveTo(getX() + 1, getY());
			m_numMovesCurrentDirection = 0;
			return true;
		}
		else if (los)
		{
			setDirection(left);
			moveTo(getX() - 1, getY());
			m_numMovesCurrentDirection = 0;
			return true;
		}
	}
	return false;
}
void Protester::lookForBribe()
{
	Actor * bribe = getWorld()->inRadius(getX(), getY(), 3, false, false, true, false);
	if (bribe == nullptr) return;
	if (!bribe->isProtesterPickup()) return;
	bribe->kill();

	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	beBribed();
}

inline void Protester::beBribed()
{
	m_leaveField = true;
	getWorld()->increaseScore(25);
}
void HardcoreProtester::beBribed()
{
	int n = (100 - getWorld()->getLevel() * 10);
	restFor((50 > n) ? 50 : n );
	getWorld()->increaseScore(50);
}

inline bool Protester::findFrackMan()
{
	return false;
}

bool HardcoreProtester::findFrackMan()
{
	if (getWorld()->checkRadius(getX(), getY(), m_huntRadius, true, false, false, false))
	{
		moveInThisDirection(getWorld()->nextDirectionToTake(this, false));
		return true;
	}
	else return false;
}

void Protester::moveInThisDirection(Direction dir)
{
	switch (dir)
	{
	case up:
		setDirection(up);
		moveTo(getX(), getY() + 1);
		break;
	case down:
		setDirection(down);
		moveTo(getX(), getY() - 1);
		break;
	case right:
		setDirection(right);
		moveTo(getX() + 1, getY());
		break;
	case left:
		setDirection(left);
		moveTo(getX() - 1, getY());
		break;
	case none:
		setDirection(right);
		if (getWorld()->isThisDirectionClear(getX(), getY(), right)) moveTo(getX() + 1, getY());
		else kill();
		return;
		break;
	}
	return;
}

////////////// Constructors/Destructors /////////////////
Protester::Protester(int startX, int startY, int ticksBetweenMoves, StudentWorld* worldin, int HP, int imageID)
	: HPActor(imageID, startX, startY, left, HP, worldin)
{
	setVisible(true);
	m_leaveField = false;
	m_numMovesCurrentDirection = 55;
	m_ticksBetweenMoves = ticksBetweenMoves;
	m_restingTicks = 0;
	m_perpendicularTicks = 200;
	m_shoutTicks = 15;
}

Protester::~Protester()
{	}

HardcoreProtester::HardcoreProtester(int startX, int startY, int ticksBetweenMoves, StudentWorld * worldin)
	:Protester(startX, startY, ticksBetweenMoves, worldin, 20, IID_HARD_CORE_PROTESTER)
{
	m_huntRadius = 16 + (worldin->getLevel() * 2);
}

HardcoreProtester::~HardcoreProtester()
{	}

//////////////////////////////////////////////////////////

void Protester::doSomething()
{
	if (!isAlive())
		return;
	
	// Resting delay
	m_restingTicks--;
	if (m_restingTicks > 0) return;
	m_restingTicks = m_ticksBetweenMoves;

	// If Protestor has been fully annoyed, it must leave the field
	if (getHP() <= 0 && m_leaveField != true)
	{
		m_restingTicks = 1;
		m_leaveField = true;
	}
	if (m_leaveField)
		if (getX() == 60 && getY() == 60)
		{
			kill();
			return;
		}
		else
		{
			moveInThisDirection(getWorld()->nextDirectionToTake(this, m_leaveField));
			return;
		}


	//Increment tick counters
	m_perpendicularTicks++;
	m_shoutTicks++;

	//Check for Gold
	lookForBribe();

	Direction dir = getDirection();
	int frackX = getWorld()->frackX();
	int frackY = getWorld()->frackY();

	if (getWorld()->checkRadius(getX(), getY(), 4, true, false, false, false)) 
	// Checks if FrackMan is near and yells if he is in current direction, otherwise turns to him
	{
		switch (dir)
		{
		case up:
			if (frackY > getY())
			{	if (m_shoutTicks >= 16) yell();	}
			else if (frackY < getY())
				setDirection(down);
			else if (frackX < getX())
				setDirection(left);
			else 
				setDirection(right);
			break;
		case down:
			if (frackY < getY())
			{	if (m_shoutTicks >= 16) yell();	}
			else if (frackY > getY())
				setDirection(up);
			else if (frackX < getX())
				setDirection(left);
			else 
				setDirection(right);
			break;
		case right:
			if (frackX > getX())
			{	if (m_shoutTicks >= 16) yell();	}
			else if (frackX < getX())
				setDirection(left);
			else if (frackY < getY())
				setDirection(down);
			else 
				setDirection(up);
			break;
		case left:
			if (frackX < getX())
			{	if (m_shoutTicks >= 16) yell();	}
			else if (frackX > getX())
				setDirection(right);
			else if (frackY < getY())
				setDirection(down);
			else 
				setDirection(up);
			break;
		}
		return;
	}

	// This function is used by Hardcore protesters to locate the FrackMan.
	// This will immediately return false for a regular protester. 
	if (findFrackMan()) return;

	// If FrackMan is in line of sight, moves in his direction.
	if(checkVerticalLineofSight(frackX, frackY)) return;
	if(checkHorizontalLineofSight(frackX, frackY)) return;

	m_numMovesCurrentDirection--;

	dir = getDirection();
	if (m_numMovesCurrentDirection <= 0) // Chooses random new direction 
	{
		Direction tempDir;
		int randDir;
		do {
			randDir = rand() % 4;
			if (randDir == 0) tempDir = up;
			if (randDir == 1) tempDir = down;
			if (randDir == 2) tempDir = right;
			if (randDir == 3) tempDir = left;
		} while (!getWorld()->isThisDirectionClear(getX(), getY(), tempDir));
		setDirection(tempDir);
		m_numMovesCurrentDirection = 55;
	}
	else if (m_perpendicularTicks >= 200)// Checks if any of the perpendicular directions are viable
	{
		int randDir = rand() % 2;
		bool upViable = (getWorld()->isThisDirectionClear(getX(), getY(), up) && getDirection() != up && getDirection() != down && getY() + 4 < 64);
		bool downViable = (getWorld()->isThisDirectionClear(getX(), getY(), down) && getDirection() != up && getDirection() != down);
		bool rightViable = (getWorld()->isThisDirectionClear(getX(), getY(), right) && getDirection() != left && getDirection() != right && getX() + 4 < 64);
		bool leftViable = (getWorld()->isThisDirectionClear(getX(), getY(), left) && getDirection() != left && getDirection() != right);
		
		// Picks random perpendicular direction if one is viable
		if (dir == up || dir == down)
		{
			if (rightViable && leftViable)
				setDirection((randDir == 0) ? left : right);
			else if (rightViable)
				setDirection(right);
			else if (downViable)
				setDirection(left);
			
		}
		else if (dir == right || dir == left)
		{
			if (upViable && downViable)
				setDirection((randDir == 0) ? up : down);
			else if (upViable)
				setDirection(up);
			else if (downViable)
				setDirection(down);
		}
		if (dir != getDirection()) m_perpendicularTicks = 0;
	}


	// Moves in its currently facing direction
	dir = getDirection();
	switch (dir)
	{
	case up:
		if (getWorld()->isThisDirectionClear(getX(), getY(), up) && getY() + 4 < 64)
			moveTo(getX(), getY()+1);
		else m_numMovesCurrentDirection = 0;
		break;
	case down:
		if (getWorld()->isThisDirectionClear(getX(), getY(), down))
			moveTo(getX(), getY()-1);
		else m_numMovesCurrentDirection = 0;
		break;
	case right:
		if (getWorld()->isThisDirectionClear(getX(), getY(), right) && getX() + 4 < 64)
			moveTo(getX()+1, getY());
		else m_numMovesCurrentDirection = 0;
		break;
	case left:
		if (getWorld()->isThisDirectionClear(getX(), getY(), left))
			moveTo(getX()-1, getY());
		else m_numMovesCurrentDirection = 0;
		break;
	}

	return;
}

////////////////////////////////////////////////////////////////////
////////////////// FrackMan Implementation /////////////////////////
////////////////////////////////////////////////////////////////////

FrackMan::FrackMan(StudentWorld * worldin)
	:HPActor(IID_PLAYER, 30, 60, right, 10, worldin) 
{
	setVisible(true);
	m_water = 5;
	m_sonars = 1;
	m_nuggets = 0;
}

FrackMan::~FrackMan()
{	}

int FrackMan::getWater() const
{	return m_water;	}

int FrackMan::getSonar() const
{	return m_sonars;	}

int FrackMan::getGold() const
{	return m_nuggets;	}


void FrackMan::squirtWater()
{
	if (m_water <= 0) return;
	
	Direction dir = getDirection();

	if (dir == up)
		if (getWorld()->isClear(getX(), getY() + 4, 4)) // first check if area is clear before spawning a squirt
			getWorld()->addActor(new Squirt(getX(), getY() + 4, getDirection(), getWorld()));
	if (dir == down)
		if (getWorld()->isClear(getX(), getY() - 4, 4))
			getWorld()->addActor(new Squirt(getX(), getY() - 4, getDirection(), getWorld()));
	if (dir == right)
		if (getWorld()->isClear(getX() + 4, getY(), 4))
			getWorld()->addActor(new Squirt(getX() + 4, getY(), getDirection(), getWorld()));
	if (dir == left)
		if (getWorld()->isClear(getX() - 4, getY(), 4))
			getWorld()->addActor(new Squirt(getX() - 4, getY(), getDirection(), getWorld()));

	getWorld()->playSound(SOUND_PLAYER_SQUIRT);
	m_water--;
	return;
}

void FrackMan::useSonar()
{
	if (m_sonars <= 0) return;

	getWorld()->revealNearbyObjects(this, 12);

	getWorld()->playSound(SOUND_SONAR);
	m_sonars--;
	return;
}

void FrackMan::doSomething()
{
	if (getHP() <= 0)
	{
		kill();
		return;
	}

	int key;
	if (getWorld()->getKey(key) == true)
	{
		switch (key)
		{
		case KEY_PRESS_ESCAPE:
			kill();
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left) setDirection(left);
			else if (getX() - 1 >= 0 && !getWorld()->checkRadius(getX() - 1, getY(), 3, false, false, false, true)) 
				moveTo(getX() - 1, getY());
			else moveTo(getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right) setDirection(right);
			else if (getX() + 1 < 61 && !getWorld()->checkRadius(getX() + 1, getY(), 3, false, false, false, true)) 
				moveTo(getX() + 1, getY());
			else moveTo(getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down) setDirection(down);
			else if (getY() - 1 >= 0 && !getWorld()->checkRadius(getX(), getY() - 1, 3, false, false, false, true)) 
				moveTo(getX(), getY() - 1);
			else moveTo(getX(), getY());
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up) setDirection(up);
			else if (getY() + 1 < 61 && !getWorld()->checkRadius(getX(), getY() + 1, 3, false, false, false, true)) 
				moveTo(getX(), getY() + 1);
			else moveTo(getX(), getY());
			break;
		case KEY_PRESS_TAB:
			dropBribe();
			break;
		case KEY_PRESS_SPACE: 
			squirtWater();
			break;
		case 'Z':
		case 'z':
			useSonar();
			break;
		}
	}
	return;
}

void FrackMan::dropBribe()
{
	if (m_nuggets <= 0) return;
	getWorld()->addActor(new BribeNugget(getX(), getY(), getWorld()));
	m_nuggets--;
}

void FrackMan::deathThroes()
{
	getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	kill();
}
void FrackMan::beAnnoyed() // FrackMan doesn't play any sound when he is annoyed.
{	return;	}

void FrackMan::addWater()
{
	m_water += 5;
}
void FrackMan::addSonar()
{
	m_sonars++;
}
void FrackMan::addGold()
{
	m_nuggets++;
}


////////////////////////////////////////////////////////////////////
/////////////////////// Just Dirt, Nothing Else. ///////////////////
////////////////////////////////////////////////////////////////////

Dirt::Dirt(int startX, int startY)
	:Actor(IID_DIRT, startX, startY, right, 0.25, 3)
{
	setVisible(true);
}

Dirt::~Dirt()
{	}

void Dirt::doSomething()
{	return;	}