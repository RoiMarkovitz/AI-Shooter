#ifndef __PLAYER_H
#define __PLAYER_H

#include "Cell.h"
#include "Point2D.h"
#include "MapCell.h"
#include "Definitions.h"
#include "Astar.h"
#include <vector>
#include <iostream>

using namespace std;

const int MAX_PLAYER_HEALTH = 100;
const int MIN_PLAYER_HEALTH = 0;
const int MAX_CRITICAL_HEALTH_LEVEL = 40;
const int MIN_CRITICAL_HEALTH_LEVEL = 25;
const int SCAN_GRENADE_AREA_SIZE = 5;
const int CHANCE_TO_STOP_ESCAPING = 20;
const int SAFE_SQUARE_SCAN_SIZE = 11;
const int SCAN_ENEMIES_SQUARE_SIZE = 21;  

class MapCell;

class Player
{
public:
	Player() = delete;
	Player(int row, int col, eTeam teamNum, int roomNum);
	Player(const Player& other) = delete;
	virtual ~Player() = 0;

	Point2D& getCurrentPoint2D() { return currentPoint2D; }
	const Point2D& getCurrentPoint2D() const { return currentPoint2D; }	
	void setCurrentPoint2D(int r, int c) { currentPoint2D.setPoint2D(r, c); }
	Point2D& getBeforePoint2D() { return beforePoint2D; }
	const Point2D& getBeforePoint2D() const { return beforePoint2D; }
	void setBeforePoint2D(int r, int c) { beforePoint2D.setPoint2D(r, c); }
	void setRoomId(int val) { roomId = val; }
	int getRoomId() const { return roomId; }
	eTeam getTeamNumber() const { return teamNumber; }
	int getId() const { return id; }
	void takeDamage(int damage);
	int getCurrentHealth() const { return currentHealth; }
	int getIsDead() const { return isDead; }
	void increaseHealth(int health);
	void setHasSeteppedInPath(bool v) { hasSteppedInPath = v; }
	bool getHasSteppedInPath() const { return hasSteppedInPath; }
	bool isSameTeam(const Player& other) const { return teamNumber == other.teamNumber; }
	const vector<Player*> getEnemiesInRange(const MapCell*const* map) const;

	const Player& operator=(const Player& other) = delete;
	const Player& operator=(Player&& other) = delete;
	bool operator == (const Player& other) const { return id == other.id; }
	virtual void toOs(ostream& os) const = 0 {}
	friend ostream& operator<<(ostream& os, const Player& p);

protected:
	static int idCounter;
	int id;
	int currentHealth;
	Point2D currentPoint2D;
	Point2D beforePoint2D;
	bool isDead;
	bool hasSteppedInPath;
	eTeam teamNumber; 
	int roomId;
	int criticalHealthLevel;

	Player* findNearestPlayerInMap(const vector<Player*>& team, const MapCell*const* map) const;
	void moveToPosition(const Point2D& targetPoint, const MapCell*const* map, eAstar option);
	void moveToSafestPosition(int width, int height, int targetRow, int targetCol, const MapCell*const* map);
	bool scanAreaForEnemyGrenades(const vector<Player*>& enemyTeam, const MapCell*const* map) const;	
};

#endif // __PLAYER_H