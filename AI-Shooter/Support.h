#ifndef __SUPPORT_H
#define __SUPPORT_H

#include "Player.h"
#include "Soldier.h"
#include "HealthStorage.h"
#include "AmmoStorage.h"
#include "MapCell.h"
#include "Astar.h"
#include "cell.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

const int MAX_HEALTH_STASHED = 200;
const int MAX_AMMO_STASHED = 20;
const int MAX_SUPPLY_HEALTH = 25;
const int MAX_SUPPLY_AMMO = 4;
const int MIN_STASH_VALUE = 0;
const double MAX_SUPPLY_RANGE = sqrt(8);
const double PICKUP_RANGE = sqrt(2);
const int AREA_AROUND_TARGET = 3;

class Support : public Player
{
private:
	enum class eMonitorDecision { Health, Ammo, CantGiveSupport, EmptyStash };

	int currentHealthStashed;
	int currentAmmoStashed;

	void support(const vector<Player*>& team, const vector<Player*>& enemyTeam, const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map);
	bool survive(const vector<Player*>& team, const vector<Player*>& enemyTeam, const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map);
	eMonitorDecision monitorTeamCondition(const vector<Player*>& team, Player** teamMemberToAid, const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map);
	void performMonitorTeamDecision(Support::eMonitorDecision decision, const vector<Player*>& team, Player* teamMemberToAid, const vector<Player*>& enemyTeam, const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map);
	void moveToTeammate(Player* teamMemberToAid, Support::eMonitorDecision decision, const MapCell*const* map);
	void moveToStorage(Storage* storage, const MapCell*const* map);
	void fillStash(const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map);
	void supplyHealth(Player& teammate);
	void supplyAmmo(Player& teammate);
	void takeHealthFromStorage(HealthStorage& healthStorage);
	void takeAmmoFromStorage(AmmoStorage& ammoStorage);
	double calculateDistanceScore(double distance) const;
	double calculateHealthScore(int health) const;
	double calculateAmmoScore(int ammo) const;

	template<class T>
	T* findNearestStorage(const vector<T*>& storages, double* minDistanceFromStorage) const
	{	
		typename vector<T*>::const_iterator itr = storages.begin();
		typename vector<T*>::const_iterator itrEnd = storages.end();
		T* storage{};
		double currentDistance;

		if (!storages.empty())
		{
			currentDistance = (*itr)->getPoint2D().calcEuclideanDistance(currentPoint2D);
			*minDistanceFromStorage = currentDistance;
			storage = (*itr);
			++itr;
		}

		for (; itr != itrEnd; ++itr)
		{
			currentDistance = (*itr)->getPoint2D().calcEuclideanDistance(currentPoint2D);
			if (currentDistance < *minDistanceFromStorage)
			{
				*minDistanceFromStorage = currentDistance;
				storage = (*itr);
			}
		}

		return storage;
	}

	template<class T>
	double calculateStorageScore(T** nearestStorage, const vector<T*>& storages, int currentStashed, int maxStashed) const
	{
		double distanceToNearrestStorage = 0, storageScore = 0, stashMissingPercentage;
		stashMissingPercentage = ((double)currentStashed / maxStashed);
		*nearestStorage = findNearestStorage(storages, &distanceToNearrestStorage);
		if (*nearestStorage)
			storageScore = abs(1 - stashMissingPercentage) + calculateDistanceScore(distanceToNearrestStorage);
		return storageScore;
	}
	
public:
	Support() = delete;
	Support(int row, int col, eTeam teamNum, int roomNum);
	Support(const Support& other) = delete;

	void playOneTurn(const vector<Player*>& team, const vector<Player*>& enemyTeam, vector<HealthStorage*>& healthStorages, vector<AmmoStorage*>& ammoStorages, const MapCell*const* map);
	double getMaxSupplyRange() const { return MAX_SUPPLY_RANGE; }
	int getCurrentAmmoStashed() const { return currentAmmoStashed; }

	const Support& operator=(const Support& other) = delete;
	const Support& operator=(Support&& other) = delete;
	virtual void toOs(ostream& os) const override;
};


#endif // __SUPPORT_H