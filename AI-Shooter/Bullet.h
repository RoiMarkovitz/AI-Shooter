#ifndef __BULLET_H
#define __BULLET_H

#include "Definitions.h"
#include "MapCell.h"
#include "Player.h"
#include <vector>

using namespace std;

class Player;
class MapCell;

const double BULLET_SIMULATION_SPEED = 0.01;
const double BULLET_SPEED = 0.5;
const double FIRE_BULLET_MAX_RANGE = 9.0;
const double MAX_BULLET_DAMAGE = 20;
const double DAMAGE_REDUCTION_RATE = 0.5;
const double MIN_BULLET_DAMAGE = 10;

class Bullet
{
public:
	Bullet() = delete;
	Bullet(double xx, double yy, double angle, eTeam owner = eTeam::null, double bulletDamage = MAX_BULLET_DAMAGE);

	void setX(double val) { x = val; }
	double getX() const { return x; }
	void setY(double val) { y = val; }
	double getY() const { return y; }
	bool move(const MapCell*const* map, const Player& shooter, bool isDummyShot);
	void setIsFired(bool status) { isFired = status; }
	void show() const;
	bool getIsFired() const { return isFired; }
	void SimulateFire(MapCell** map, double damage);
	bool isEnemyHitableByBullet(const MapCell*const* map, const Player& attacker);
	bool isOuterMazeWallInPath(const MapCell*const* map, double fireDistance);

private:
	double x, y;
	double angleDirection;
	double dx, dy;
	double distancePassed;
	bool isFired;
	double damage;
	eTeam ownerTeam; // the team which owns the bullet
	
	double getDistancedPassed() const { return distancePassed; }
};

#endif // __BULLET_H

