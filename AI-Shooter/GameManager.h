#ifndef __GAME_MANAGER_H
#define __GAME_MANAGER_H

#include "Definitions.h"
#include "Player.h"
#include "Support.h"
#include "Soldier.h"
#include "HealthStorage.h"
#include "AmmoStorage.h"
#include "CreateMap.h"
#include "MapCell.h"
#include <vector>
#include <iostream>

using namespace std;

const double SIMULATION_DAMAGE = 0.005; 

class GameManager
{
private:
	MapCell** map;
	vector<Point2D*> centerPositionsMapCellsToClean;
	vector<Player*> team1;
	vector<Player*> team2;
	vector<HealthStorage*> healthStorages; 
	vector<AmmoStorage*> ammoStorages; 
	bool hasGameStarted; 
	bool hasGameEnded; 

	void playTeamTurn(const vector<Player*>& team, const vector<Player*>& enemyTeam);
	void updateLocationsInMap();
	void updateTeamProjectiles(const vector<Player*>& team) const;
	void updateTeamStatus(vector<Player*>& team);
	void updateTeamLocations(const vector<Player*>& team) const;
	void updateRiskLevelMap(const Player& player);
	void clearRiskLevelMap();
	void checkIfGameHasEnded();
	bool isAllTeamDead(const vector<Player*>& team) const;
	bool isOnlySupportLeftInTeam(const vector<Player*>& team) const;
	bool noActivePorjectilesForTeam(const vector<Player*>& team) const;
	bool noMoreAmmoForTeam(const vector<Player*>& team) const;

	template<class T>
	void updateStoragesCondition(vector<T*>& storages)
	{
		typename vector<T*>::iterator itr = storages.begin();
		T* tempStorage;
		int row, col;
		while (itr != storages.end())
		{
			tempStorage = (*itr);
			row = tempStorage->getPoint2D().getRow();
			col = tempStorage->getPoint2D().getCol();
			if (tempStorage->getIsEmpty())
			{
				map[row][col].setMark(MapMarks::OBSTACLE);
				map[row][col].setStorageInCell(nullptr);
				itr = storages.erase(itr);
			}
			else
				++itr;
		}
	}
	
public:
	GameManager();
	GameManager(const GameManager& other) = delete;
	~GameManager();

	void playOneTurn();
	void startGame() { hasGameStarted = true; }
	bool getHasGameStarted() const { return hasGameStarted; }
	bool getHasGameEnded() const { return hasGameEnded; }
	const MapCell*const* getMap() const { return map; }	
	const vector<Player*>& getTeam1() const { return team1; }
	const vector<Player*>& getTeam2() const { return team2; }

	const GameManager& operator=(const GameManager& other) = delete;
	const GameManager& operator=(GameManager&& other) = delete;
};

#endif // __GAME_MANAGER_H