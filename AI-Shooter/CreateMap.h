#ifndef __CREATE_MAP_H
#define __CREATE_MAP_H

#include "Definitions.h"
#include "Room.h"
#include "Player.h"
#include "HealthStorage.h"
#include "AmmoStorage.h"
#include "MapCell.h"
#include "Astar.h"
#include "Grenade.h"
#include "Soldier.h"
#include "Support.h"
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

const int CORRIDOR_WIDTH_HEIGHT = 5;

// util class to handle creating the map

class CreateMap
{
private:
	static void initMap(MapCell*** map, int height, int width);
	static void initRooms(Room*** rooms, MapCell** map, vector<Player*>& team1, vector<Player*>& team2, vector<HealthStorage*>& healthStorages, vector<AmmoStorage*>& ammoStorages);
	static bool hasOverlapping(Room** rooms, int index, int w, int h, int crow, int ccol);
	static void fillRoom(Room* room, MapCell** map, vector<HealthStorage*>& healthStorages, vector<AmmoStorage*>& ammoStorages);
	static void placeTeam(Room* room, vector<Player*>& team, MapCell** map, eTeam teamNum);
	static void digTunnels(Room** rooms, MapCell** map);
	static void buildPath(Cell* pCell, MapCell** map);
	static void clearMemory(Room** rooms);

public:
	CreateMap() = delete;
	static void createMap(MapCell*** map, vector<Player*>& team1, vector<Player*>& team2, vector<HealthStorage*>& healthStorages, vector<AmmoStorage*>& ammoStorages);
};

#endif // __CREATE_MAP_H
