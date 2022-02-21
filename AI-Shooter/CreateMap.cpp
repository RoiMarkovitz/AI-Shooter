#include "CreateMap.h"

void CreateMap::createMap(MapCell*** map, vector<Player*>& team1, vector<Player*>& team2, vector<HealthStorage*>& healthStorages, vector<AmmoStorage*>& ammoStorages)
{
	Room** rooms; 
	initMap(map, MSZ, MSZ);
	initRooms(&rooms, *map, team1, team2, healthStorages, ammoStorages);
	digTunnels(rooms, *map);
	clearMemory(rooms);
}

void CreateMap::initMap(MapCell*** map, int height, int width)
{
	*map = new MapCell* [height]; // allocating the rows
	for (int i = 0; i < height; i++)
	{
		(*map)[i] = new MapCell[width]; // allocating the columns
		for (int j = 0; j < width; j++)
		{
			(*map)[i][j].setMark(MapMarks::OUTER_WALL);
			(*map)[i][j].setPosition(i, j);
		}								
	}
}

void CreateMap::initRooms(Room*** rooms, MapCell** map, vector<Player*>& team1, vector<Player*>& team2, vector<HealthStorage*>& healthStorages, vector<AmmoStorage*>& ammoStorages)
{
	*rooms = new Room* [NUM_ROOMS];
	int crow, ccol, w, h;
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		// init a room
		do {
			w = MIN_ROOM_WIDTH + rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1);
			h = MIN_ROOM_HEIGHT + rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1);
			crow = 2 + h / 2 + rand() % (MSZ - (5 + h));
			ccol = 2 + w / 2 + rand() % (MSZ - (5 + w));
		} while (hasOverlapping(*rooms, i, w, h, crow, ccol));
		(*rooms)[i] = new Room(crow, ccol, w, h, i);
		fillRoom((*rooms)[i], map, healthStorages, ammoStorages);
	}

	placeTeam((*rooms)[0], team1, map, eTeam::Team1);
	placeTeam((*rooms)[NUM_ROOMS-1], team2, map, eTeam::Team2);	
}

bool CreateMap::hasOverlapping(Room** rooms, int index, int w, int h, int crow, int ccol)
{
	int dx, dy;
	bool overlap = false;
	for (int i = 0; i < index && !overlap; i++)
	{
		dx = abs(ccol - rooms[i]->getCenterPoint().getCol());
		dy = abs(crow - rooms[i]->getCenterPoint().getRow());
		if (dx < w / 2 + rooms[i]->getW() / 2 + 3 &&
			dy < h / 2 + rooms[i]->getH() / 2 + 3)
			overlap = true;
	}
	return overlap;
}

void CreateMap::fillRoom(Room* room, MapCell** map, vector<HealthStorage*>& healthStorages, vector<AmmoStorage*>& ammoStorages)
{
	int cr, cc, w, h;
	int randRow, randCol;
	int roomSize;

	w = room->getW();
	h = room->getH();
	cr = room->getCenterPoint().getRow();
	cc = room->getCenterPoint().getCol();
	roomSize = w * h;
	// set room space
	for (int i = cr - h / 2; i <= cr + h / 2; i++)
		for (int j = cc - w / 2; j <= cc + w / 2; j++)
		{
			map[i][j].setMark(MapMarks::SPACE);
			map[i][j].setRoomId(room->getRoomId());
		}
			
	// set random obstacles
	for (int i = cr - (h / 2) + 1; i <= cr + (h / 2) - 1; i++)
		for (int j = cc - (w / 2) + 1; j <= cc + (w / 2) - 1; j++)
			if (rand() % 100 < OBTACLES_PERCENTAGE_IN_ROOM)
				map[i][j].setMark(MapMarks::OBSTACLE);
	
	for (int i = 0; i < NUM_HEALTH_KITS_IN_ROOM; i++)
	{
		do
		{
			// (rand() % (upper -1 – lower )) + lower + 1
			randRow = (rand() % ((cr + (h / 2) - 1) - (cr - h / 2))) + (cr - h / 2) + 1;
			randCol = (rand() % ((cc + (w / 2) - 1) - (cc - w / 2))) + (cc - w / 2) + 1;
		} while (map[randRow][randCol].getMark() != MapMarks::SPACE);
		map[randRow][randCol].setMark(MapMarks::HEALTH_STORAGE);
		healthStorages.push_back(new HealthStorage(randRow, randCol));
		map[randRow][randCol].setStorageInCell(healthStorages.back());
	}

	for (int i = 0; i < NUM_AMMO_KITS_IN_ROOM; i++)
	{
		do
		{
			// (rand() % (upper -1 – lower )) + lower + 1
			randRow = (rand() % ((cr + (h / 2) - 1) - (cr - h / 2))) + (cr - h / 2) + 1;
			randCol = (rand() % ((cc + (w / 2) - 1) - (cc - w / 2))) + (cc - w / 2) + 1;
		} while (map[randRow][randCol].getMark() != MapMarks::SPACE);
		map[randRow][randCol].setMark(MapMarks::AMMO_STORAGE);
		ammoStorages.push_back(new AmmoStorage(randRow, randCol));
		map[randRow][randCol].setStorageInCell(ammoStorages.back());
	}
}

void CreateMap::placeTeam(Room* room, vector<Player*>& team, MapCell** map, eTeam teamNum)
{
	int cr, cc, w, h;
	int randRow, randCol;
	int roomId;

	w = room->getW();
	h = room->getH();
	cr = room->getCenterPoint().getRow();
	cc = room->getCenterPoint().getCol();
	roomId = room->getRoomId();

	for (int i = 0; i < TEAM_SIZE; i++)
	{
		do
		{
			// (rand() % (upper – lower + 1)) + lower
			randRow = (rand() % ((cr + h / 2) - (cr - h / 2) + 1)) + (cr - h / 2);
			randCol = (rand() % ((cc + w / 2) - (cc - w / 2) + 1)) + (cc - w / 2);
		} while (map[randRow][randCol].getMark() != MapMarks::SPACE);
		if (teamNum == eTeam::Team1)
			if (i == 0)
			{
				team.push_back(new Support(randRow, randCol, eTeam::Team1, roomId));
				map[randRow][randCol].setMark(MapMarks::TEAM_ONE_SUPPORT);			
			}			
			else
			{
				team.push_back(new Soldier(randRow, randCol, eTeam::Team1, roomId, (Support*)team.front()));
				map[randRow][randCol].setMark(MapMarks::TEAM_ONE_SOLDIER);
			}			
		else if (i == 0)
		{
			team.push_back(new Support(randRow, randCol, eTeam::Team2, roomId));
			map[randRow][randCol].setMark(MapMarks::TEAM_TWO_SUPPORT);		
		}		
		else
		{
			team.push_back(new Soldier(randRow, randCol, eTeam::Team2, roomId, (Support*)team.front()));
			map[randRow][randCol].setMark(MapMarks::TEAM_TWO_SOLDIER);
		}	
		map[randRow][randCol].setPlayerInCell(team.back());
	}
}

void CreateMap::digTunnels(Room** rooms, MapCell** map)
{
	int i, j;
	Cell* pCell;
	for (i = 0; i < NUM_ROOMS; i++)
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			pCell = Astar::aStar(rooms[i]->getCenterPoint(), rooms[j]->getCenterPoint(), map, eAstar::BuildCorridors); // A*
			if(pCell)
				buildPath(pCell, map);
			Astar::freePathMemory(pCell);
		}
}

void CreateMap::buildPath(Cell* pCell, MapCell** map)
{
	while (pCell->getParent() != nullptr)
	{
		if (map[pCell->getPosition().getRow()][pCell->getPosition().getCol()].getMark() == MapMarks::OUTER_WALL)
		{
			for (int r = pCell->getPosition().getRow() - CORRIDOR_WIDTH_HEIGHT / 2; r <= pCell->getPosition().getRow() + CORRIDOR_WIDTH_HEIGHT / 2; r++)
			{
				for (int c = pCell->getPosition().getCol() - CORRIDOR_WIDTH_HEIGHT / 2; c <= pCell->getPosition().getCol() + CORRIDOR_WIDTH_HEIGHT / 2; c++)
				{
					if (map[r][c].getMark() == MapMarks::OUTER_WALL)
					{
						map[r][c].setMark(MapMarks::CORRIDOR);
						map[r][c].setRoomId(CORRIDOR_ID);
					}			
				}
			}
		}							
		pCell = pCell->getParent();
	}	
}

void CreateMap::clearMemory(Room** rooms)
{
	for (int i = 0; i < NUM_ROOMS; i++)
		delete rooms[i];
	delete[] rooms;
}



