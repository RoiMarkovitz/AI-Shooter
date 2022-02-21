#ifndef __MAP_CELL_H
#define __MAP_CELL_H

#include "Definitions.h"
#include "Storage.h"
#include "Player.h"
#include "Grenade.h"
#include <vector>

using namespace std;

class Grenade;
class Player;

const double MIN_RISK_LEVEL = 1;

class MapCell
{
private:
	int roomId;
	double riskLevel;
	Point2D position;
	MapMarks mark;
	Player* player;
	Storage* storage;

	MapCell();

public:
	MapCell(int r, int c, MapMarks m);
	Point2D& getPosition() { return position; }
	void setPosition(int row, int col) { position.setPoint2D(row, col); }
	const Point2D& getPosition() const { return position; }
	void setPlayerInCell(Player* p) { player = p; }
	Player* getPlayerInCell() const { return player; }
	void setStorageInCell(Storage* s) { storage = s; }
	Storage* getStorageInCell() const { return storage; }
	void setMark(MapMarks m) { mark = m; }
	MapMarks getMark() const { return mark; }
	void setRoomId(int val) { roomId = val; }
	int getRoomId() const { return roomId; }
	void setRiskLevel(double val) { riskLevel = val; }
	double getRiskLevel() const { return riskLevel; }
	bool operator == (const MapCell& other) const { return position == other.position ; }

	friend class CreateMap;
};

#endif // __MAP_CELL_H