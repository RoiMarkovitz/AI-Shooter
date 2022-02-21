#ifndef __GRENADE_H
#define __GRENADE_H

#include "Bullet.h"
#include "MapCell.h"
#include "Player.h"
#include "Point2D.h"
#include <vector>

class MapCell;
class Bullet;

const int NUM_BULLETS = 8;
const double THROW_GRENADE_MAX_RANGE = 5.0;
const double THROW_GRENADE_MIN_RANGE = 4.0;
const double GRENADE_SPEED = 0.5;
const double PARTICLES_DAMAGE = 10;

using namespace std;

class Grenade
{
private:
	vector<Bullet*> bullets;
	int numBullets;
	double angleDirection;
	double dx, dy;
	double x, y;
	Point2D targetPosition; 
	bool isDetonated;
	bool didAllParticlesHitSomething;
	eTeam ownerTeam; // the team which owns the grenade

	bool didAllParticlesHit() const;

public:
	Grenade() = delete;
	Grenade(double xx, double yy, int numOfBullets = NUM_BULLETS, double particleDamage = PARTICLES_DAMAGE);
	Grenade(double xx, double yy, Point2D& targetPos, double angle = 0, eTeam owner = eTeam::null, int numOfBullets = NUM_BULLETS, double particleDamage = PARTICLES_DAMAGE);
	Grenade(const Grenade& other);
	Grenade(Grenade&& other) noexcept;
	~Grenade();

	bool getIsDetonated() const { return isDetonated; }	
	bool getDidAllParticlesHitSomething() const { return didAllParticlesHitSomething; }
	void show() const;
	void moveGrenadeParticles(const MapCell* const* map, const Player& attacker);
	void moveGrenadeToTarget(const MapCell* const* map);
	void SimulateExplosion(MapCell** map, double damage);
	eTeam getOwnerTeam() const { return ownerTeam; }
	Point2D& getTargetPosition() { return targetPosition; }
	const Point2D& getTargetPosition() const { return targetPosition; }
	void setX(double val) { x = val; }
	double getX() const { return x; }
	void setY(double val) { y = val; }
	double getY() const { return y; }

	const Grenade& operator=(const Grenade& other);
	const Grenade& operator=(Grenade&& other) noexcept;	
};

#endif // __GRENADE_H