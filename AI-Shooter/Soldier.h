#ifndef __SOLDIER_H
#define __SOLDIER_H

#include "Player.h"
#include "MapCell.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Astar.h"
#include "cell.h"
#include <vector>
#include "Support.h"
#include <iostream>

using namespace std;

const int MIN_AMMO_SOLDIER = 0;
const int MAX_AMMO_SOLDIER = 16;
const int GRENADE_AMMO_COST = 4;
const int MIN_CRITICAL_AMMO_LEVEL = 3;
const int MAX_CRITICAL_AMMO_LEVEL = 5;
const int MIN_ATTEMPT_TO_SHOOT_CHANCE = 60;
const int MAX_ATTEMPT_TO_SHOOT_CHANCE = 80;
const int MAX_TURNS_TO_STOP_ESCAPE = 4;

class Support;

class Soldier : public Player
{
private:
	int currentAmmo;
	Bullet* bullet;
	Grenade* grenade;
	Support* support;
	int criticalAmmoLevel;
	int countTurnsStopEscape;

	bool war(const vector<Player*>& enemyTeam, const MapCell*const* map);
	bool survive(const vector<Player*>& enemyTeam, const Support* support, const MapCell*const* map);
	bool shoot(const Player& enemy, const MapCell*const* map);
	bool isShootingAllowed(const Player& enemy, const MapCell*const* map) const;
	bool throwGrenade(const Player& targetEnemy, const vector<Player*>& enemyTeam, const MapCell*const* map);
	bool isThrowingGrenadeAllowed(const Player& targetEnemy, const vector<Player*>& enemyTeam, const MapCell*const* map) const;
	const Player* findNearestVisibleEnemyInRange(const vector<Player*>& enemyTeamInRange, const MapCell*const* map) const;
	const Player* findEnemy(const vector<Player*>& enemyTeam, const MapCell*const* map) const;

public:
	Soldier() = delete;
	Soldier(int row, int col, eTeam teamNum, int roomNum, Support* mySupport = nullptr);
	Soldier(const Soldier& other) = delete;
	~Soldier();

	void playOneTurn(const vector<Player*>& enemyTeam, const MapCell*const* map);
	int getCurrentAmmo() const { return currentAmmo; };
	void increaseAmmo(int ammo);
	void decreaseAmmo(int ammo);
	void setSupport(Support* mySupport) { support = mySupport; }
	void moveBullet(const MapCell*const* map);
	void moveGrenade(const MapCell*const* map);	
	Bullet* getBullet() const { return bullet; }
	Grenade* getGrenade() const { return grenade; }

	const Soldier& operator=(const Soldier& other) = delete;
	const Soldier& operator=(Soldier&& other) = delete;
	virtual void toOs(ostream& os) const override;
};

#endif // __SOLDIER_H