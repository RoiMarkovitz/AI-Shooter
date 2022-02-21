#include "Grenade.h"

Grenade::Grenade(double xx, double yy, int numOfBullets, double particleDamage)
{
	int i;
	x = xx;
	y = yy;
	numBullets = numOfBullets;
	
	double angle, teta = (2 * PI) / numBullets;
	for (i = 0, angle = 0; i < numBullets; i++, angle += teta)
		bullets.push_back(new Bullet(x, y, angle, eTeam::null, particleDamage));

	isDetonated = false;
	didAllParticlesHitSomething = false;
}

Grenade::Grenade(double xx, double yy, Point2D& targetPos, double angleDir, eTeam ownedByTeam, int numOfBullets, double particleDamage) 
{
	targetPosition = targetPos;
	int i;	
	angleDirection = angleDir; // in radians
	dx = cos(angleDirection);
	dy = sin(angleDirection);
	x = xx + 0.5;
	y = yy + 0.5;
	numBullets = numOfBullets;
	ownerTeam = ownedByTeam;
	double angle,teta = (2 * PI) / numBullets; 
	for (i = 0, angle = 0; i < numBullets; i++, angle += teta)
	{
		bullets.push_back(new Bullet(x, y, angle, ownerTeam, particleDamage));
		bullets.back()->setIsFired(true);
	}
				
	isDetonated = false;
	didAllParticlesHitSomething = false;
}

Grenade::Grenade(const Grenade& other)
{
	*this = other;
}

Grenade::Grenade(Grenade&& other) noexcept
{
	*this = std::move(other);
}

const Grenade& Grenade::operator=(const Grenade& other)
{
	if (this != &other)
	{
		numBullets = other.numBullets;
		angleDirection = other.angleDirection;
		dx = other.dx;
		dy = other.dy;
		x = other.x;
		y = other.y;
		targetPosition = other.targetPosition;
		isDetonated = other.isDetonated;
		didAllParticlesHitSomething = other.didAllParticlesHitSomething;
		ownerTeam = other.ownerTeam;
			
		bullets.clear();
		vector<Bullet*>::const_iterator itr = other.bullets.begin();
		vector<Bullet*>::const_iterator itrEnd = other.bullets.end();
		for (int i = 0; itr != itrEnd; ++itr, i++)
			bullets.push_back(new Bullet(*(*itr)));
	}

	return *this;
}

const Grenade& Grenade::operator=(Grenade&& other) noexcept 
{
	if (this != &other)
	{
		numBullets = other.numBullets;
		angleDirection = other.angleDirection;
		dx = other.dx;
		dy = other.dy;
		x = other.x;
		y = other.y;
		targetPosition = other.targetPosition;
		isDetonated = other.isDetonated;
		didAllParticlesHitSomething = other.didAllParticlesHitSomething;
		ownerTeam = other.ownerTeam;

		swap(bullets, other.bullets);
	}

	return *this;
}

Grenade::~Grenade()
{
	vector<Bullet*>::iterator itr = bullets.begin();
	vector<Bullet*>::iterator itrEnd = bullets.end();
	for (; itr != itrEnd; ++itr)
		delete *itr;
}

bool Grenade::didAllParticlesHit() const
{
	vector<Bullet*>::const_iterator itr = bullets.begin();
	vector<Bullet*>::const_iterator itrEnd = bullets.end();
	for (; itr != itrEnd; ++itr)
		if ((*itr)->getIsFired())
			return false;

	return true;
}

// detonate real grenade and move its particles
void Grenade::moveGrenadeParticles(const MapCell*const* map, const Player& attacker)
{
	vector<Bullet*>::iterator itr = bullets.begin();
	vector<Bullet*>::iterator itrEnd = bullets.end();

	if (isDetonated)
	{
		for (; itr != itrEnd; ++itr)
			(*itr)->move(map, attacker, false);

		didAllParticlesHitSomething = didAllParticlesHit();
	}
}

// move real grenade to designated explotion location
void Grenade::moveGrenadeToTarget(const MapCell*const* map)
{
	if (!isDetonated)
	{	
		vector<Bullet*>::iterator itr = bullets.begin();
		vector<Bullet*>::iterator itrEnd = bullets.end();
		for (; itr != itrEnd; ++itr)
		{
			// update x,y for each bullet
			(*itr)->setX((*itr)->getX() + dx * GRENADE_SPEED);
			(*itr)->setY((*itr)->getY() + dy * GRENADE_SPEED);							
		}
		// update x,y for grenade
		x += dx * GRENADE_SPEED;
		y += dy * GRENADE_SPEED;

		if (targetPosition.getRow() == (int)y && targetPosition.getCol() == (int)x)
		{
			isDetonated = true;
			return;
		}
	}
}

void Grenade::show() const
{
	vector<Bullet*>::const_iterator itr = bullets.begin();
	vector<Bullet*>::const_iterator itrEnd = bullets.end();
	for (; itr != itrEnd; ++itr)
		(*itr)->show(); 
}

void Grenade::SimulateExplosion(MapCell** map, double damage)
{
	vector<Bullet*>::iterator itr = bullets.begin();
	vector<Bullet*>::iterator itrEnd = bullets.end();
	for (; itr != itrEnd; ++itr)
	{
		(*itr)->setIsFired(true);
		(*itr)->SimulateFire(map, damage);
	}
}


