#include "Support.h"

Support::Support(int row, int col, eTeam teamNum, int roomNum) : Player(row, col, teamNum, roomNum)
{
	currentHealthStashed = MAX_HEALTH_STASHED;  
	currentAmmoStashed = MAX_AMMO_STASHED;  
}

void Support::playOneTurn(const vector<Player*>& team, const vector<Player*>& enemyTeam, vector<HealthStorage*>& healthStorages, vector<AmmoStorage*>& ammoStorages, const MapCell*const* map)
{
	if (!isDead)
	{
		if (scanAreaForEnemyGrenades(enemyTeam, map))
			moveToSafestPosition(SAFE_SQUARE_SCAN_SIZE, SAFE_SQUARE_SCAN_SIZE, currentPoint2D.getRow(), currentPoint2D.getCol(), map);
		else if (!survive(team, enemyTeam, healthStorages, ammoStorages, map))
			support(team, enemyTeam, healthStorages, ammoStorages, map);
	}	
}

void Support::support(const vector<Player*>& team, const vector <Player*>& enemyTeam, const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map)
{
	cout << *this << ", SUPPORT Mode" << endl;
	Player* teamMemberToAid{};
	eMonitorDecision teamMonitorDecision;
	teamMonitorDecision = monitorTeamCondition(team, &teamMemberToAid, healthStorages, ammoStorages, map);
	performMonitorTeamDecision(teamMonitorDecision, team, teamMemberToAid, enemyTeam, healthStorages, ammoStorages, map);
}

bool Support::survive(const vector<Player*>& team, const vector<Player*>& enemyTeam, const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map)
{
	HealthStorage* nearestHealthStorage;
	double distanceToNearrestHealthStorage;
	if (criticalHealthLevel >= currentHealth && currentHealthStashed == MIN_STASH_VALUE)
	{   // health is critical and health stashed is empty
		cout << *this << ", SURVIVAL Mode" << endl;
		nearestHealthStorage = findNearestStorage(healthStorages, &distanceToNearrestHealthStorage);
		if (nearestHealthStorage) // health storage was found
			moveToStorage(nearestHealthStorage, map);
		else // there are no health storages left	
			support(team, enemyTeam, healthStorages, ammoStorages, map);	
		return true;
	}
	else // not in survival mode
		return false;
}

Support::eMonitorDecision Support::monitorTeamCondition(const vector<Player*>& team, Player** teamMemberToAid, const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map)
{
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	Player* tempTeamMember;
	Soldier* tempSoldier;
	Cell* pCell;
	int health, ammo;
	double distanceFromTeamMember, tempHealthScore = 0, tempAmmoScore = 0, currentScore = 0, maxScore = 0;
	Support::eMonitorDecision decision = Support::eMonitorDecision::CantGiveSupport;

	if (currentHealthStashed == MIN_STASH_VALUE && currentAmmoStashed == MIN_STASH_VALUE
		&& !healthStorages.empty() && !ammoStorages.empty())
		return Support::eMonitorDecision::EmptyStash;
	
	for (; itr != itrEnd; ++itr)
	{	
		tempTeamMember = (*itr);	
		if (!tempTeamMember->getIsDead())
		{
			pCell = Astar::aStar(currentPoint2D, tempTeamMember->getCurrentPoint2D(), map, eAstar::ShortestPath);
			if (pCell)
			{
				distanceFromTeamMember = pCell->getF();
				Astar::freePathMemory(pCell);
				currentScore = calculateDistanceScore(distanceFromTeamMember);

				health = tempTeamMember->getCurrentHealth();
				if (currentHealthStashed > MIN_STASH_VALUE)
					tempHealthScore = calculateHealthScore(health);

				tempSoldier = dynamic_cast<Soldier*>((*itr));
				if (tempSoldier)
				{
					ammo = tempSoldier->getCurrentAmmo();
					if (currentAmmoStashed > MIN_STASH_VALUE)
						tempAmmoScore = calculateAmmoScore(ammo);
				}

				if (tempHealthScore == 0 && tempAmmoScore == 0)
					continue;
				else
					currentScore += max(tempHealthScore, tempAmmoScore);

				if (maxScore <= currentScore)
				{
					maxScore = currentScore;
					*teamMemberToAid = tempTeamMember;
					if (tempSoldier && tempHealthScore < tempAmmoScore)
						decision = Support::eMonitorDecision::Ammo;
					else // support or soldier with more health score
						decision = Support::eMonitorDecision::Health;
				}	
				tempHealthScore = 0; tempAmmoScore = 0;
			}	
		}	
	} // for
	
	return decision;	
}

void Support::performMonitorTeamDecision(Support::eMonitorDecision decision, const vector<Player*>& team, Player* teamMemberToAid, const vector<Player*>& enemyTeam, const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map)
{
	switch ((int)decision)
	{
		case ((int)Support::eMonitorDecision::Health):
			cout << "Going to give health to: " << *teamMemberToAid << endl;
			moveToTeammate(teamMemberToAid, decision, map);
			break;
		case ((int)Support::eMonitorDecision::Ammo):
			cout << "Going to give ammo to: " << *teamMemberToAid << endl;
			moveToTeammate(teamMemberToAid, decision, map);
			break;
		case ((int)Support::eMonitorDecision::CantGiveSupport):	   
			if (currentHealthStashed == MAX_HEALTH_STASHED && currentAmmoStashed == MAX_AMMO_STASHED
				|| (healthStorages.empty() && ammoStorages.empty()))
			{    // ammo and health stashes are full OR no more health and ammo storages in game
				cout << "Looking for nearest team member. Health and ammo are full for all team OR all storages are empty" << endl;
				teamMemberToAid = findNearestPlayerInMap(team, map);
				if (teamMemberToAid)
					moveToTeammate(teamMemberToAid, decision, map);
				else if (rand() % 100 >= CHANCE_TO_STOP_ESCAPING)
					moveToSafestPosition(SAFE_SQUARE_SCAN_SIZE, SAFE_SQUARE_SCAN_SIZE, currentPoint2D.getRow(), currentPoint2D.getCol(), map);
			}		
			else // specific supply that teammate needs is missing, need to fill relevant stash
			{
				cout << "Specific supply that team member needs is missing. need to fill relevant stash" << endl;
				fillStash(healthStorages, ammoStorages, map);
			}
			break;
		case ((int)Support::eMonitorDecision::EmptyStash):
			fillStash(healthStorages, ammoStorages, map);
			break;
	}
}

void Support::fillStash(const vector<HealthStorage*>& healthStorages, const vector<AmmoStorage*>& ammoStorages, const MapCell*const* map)
{
	HealthStorage* nearestHealthStorage{};
	AmmoStorage* nearestAmmoStorage{};
	double healthStorageScore = 0, ammoStorageScore = 0;

	healthStorageScore = calculateStorageScore(&nearestHealthStorage, healthStorages, currentHealthStashed, MAX_HEALTH_STASHED);
	ammoStorageScore = calculateStorageScore(&nearestAmmoStorage, ammoStorages, currentAmmoStashed, MAX_AMMO_STASHED);
	
	if (healthStorageScore >= ammoStorageScore)
		moveToStorage(nearestHealthStorage, map);
	else
		moveToStorage(nearestAmmoStorage, map);
}

void Support::moveToStorage(Storage* storage, const MapCell*const* map)
{
	if (storage)
	{
		moveToSafestPosition(AREA_AROUND_TARGET, AREA_AROUND_TARGET, storage->getPoint2D().getRow(), storage->getPoint2D().getCol(), map);
		
		if (dynamic_cast<HealthStorage*>(storage))
		{
			cout << "Going to storage fill HEALTH stash" << endl;
			takeHealthFromStorage(*(HealthStorage*)storage);
		}		
		else
		{
			cout << "Going to storage fill AMMO stash" << endl;
			takeAmmoFromStorage(*(AmmoStorage*)storage);
		}		
	}
}

void Support::moveToTeammate(Player* teamMemberToAid, Support::eMonitorDecision decision, const MapCell*const* map)
{
	if (teamMemberToAid)
	{
		moveToSafestPosition(AREA_AROUND_TARGET, AREA_AROUND_TARGET, teamMemberToAid->getCurrentPoint2D().getRow(), teamMemberToAid->getCurrentPoint2D().getCol(), map);
		
		if (eMonitorDecision::Health == decision)
			supplyHealth(*teamMemberToAid);
		else if (eMonitorDecision::Ammo == decision)
			supplyAmmo(*teamMemberToAid);
	}
}

void Support::supplyHealth(Player& teammate)
{
	if (MAX_SUPPLY_RANGE >= currentPoint2D.calcEuclideanDistance(teammate.getCurrentPoint2D()) 
		&& teamNumber == teammate.getTeamNumber())
	{
		int healthMissing, healthToSupply;
		healthMissing = MAX_PLAYER_HEALTH - teammate.getCurrentHealth();
		healthToSupply = healthMissing <= currentHealthStashed ? healthMissing : currentHealthStashed;
		healthToSupply = healthToSupply >= MAX_SUPPLY_HEALTH ? MAX_SUPPLY_HEALTH : healthToSupply;
		teammate.increaseHealth(healthToSupply);
		currentHealthStashed -= healthToSupply;
	}
}
                    
void Support::supplyAmmo(Player& teammate)
{
	int ammoMissing, ammoToSupply;
	Soldier* tempSoldier = dynamic_cast<Soldier*>(&teammate); 
	if (tempSoldier && MAX_SUPPLY_RANGE >= currentPoint2D.calcEuclideanDistance(teammate.getCurrentPoint2D()) 
		&& teamNumber == teammate.getTeamNumber())
	{	
		ammoMissing = MAX_AMMO_SOLDIER - tempSoldier->getCurrentAmmo();
		ammoToSupply = ammoMissing <= currentAmmoStashed ? ammoMissing : currentAmmoStashed;
		ammoToSupply = ammoToSupply >= MAX_SUPPLY_AMMO ? MAX_SUPPLY_AMMO : ammoToSupply;
		tempSoldier->increaseAmmo(ammoToSupply);
		currentAmmoStashed -= ammoToSupply;
	}
}

void Support::takeHealthFromStorage(HealthStorage& healthStorage)
{
	if (PICKUP_RANGE >= currentPoint2D.calcEuclideanDistance(healthStorage.getPoint2D()))
	{
		int healthInStorage, healthMissing, healthToTake;
		healthInStorage = healthStorage.getHealthInStorage();
		healthMissing = MAX_HEALTH_STASHED - currentHealthStashed;
		healthToTake = healthMissing <= healthInStorage ? healthMissing : healthInStorage;
		healthStorage.decreaseHealth(healthToTake);
		currentHealthStashed += healthToTake;
	}
}

void Support::takeAmmoFromStorage(AmmoStorage& ammoStorage)
{
	if (PICKUP_RANGE >= currentPoint2D.calcEuclideanDistance(ammoStorage.getPoint2D()))
	{
		int ammoInStorage, ammoMissing, ammoToTake;
		ammoInStorage = ammoStorage.getAmmoInStorage();
		ammoMissing = MAX_AMMO_STASHED - currentAmmoStashed;
		ammoToTake = ammoMissing <= ammoInStorage ? ammoMissing : ammoInStorage;
		ammoStorage.decreaseAmmo(ammoToTake);
		currentAmmoStashed += ammoToTake;
	}
}

double Support::calculateDistanceScore(double distance) const
{
	double distanceScore{};

	if (distance <= 5)
		distanceScore = 1;
	else if (10 >= distance && distance > 5)
		distanceScore = 0.75;
	else if (15 >= distance && distance > 10)
		distanceScore = 0.5;
	else if (20 >= distance && distance > 15)
		distanceScore = 0.25;
	else
		distanceScore = 0.1;

	return distanceScore;
}

double Support::calculateHealthScore(int health) const
{
	double healthScore{};

	if (health <= 20)
		healthScore = 1;
	else if (40 >= health && health > 20)
		healthScore = 0.8;
	else if (60 >= health && health > 40)
		healthScore = 0.5;
	else if (85 >= health && health > 60)
		healthScore = 0.3;
	else if (MAX_PLAYER_HEALTH > health && health > 85)
		healthScore = 0.1;
	else // health is full already
		healthScore = 0;

	return healthScore;
}
double Support::calculateAmmoScore(int ammo) const
{
	double ammoScore{};

	if (ammo <= 4 && ammo >= 0)
		ammoScore = 0.9;
	else if (8 >= ammo && ammo > 4)
		ammoScore = 0.6;
	else if (12 >= ammo && ammo > 8)
		ammoScore = 0.3;
	else if (MAX_AMMO_SOLDIER > ammo && ammo > 12)
		ammoScore = 0.15;
	else // ammo is full already
		ammoScore = 0;

	return ammoScore;
}

void Support::toOs(ostream& os) const
{
	os << ", HealthStashed: " << currentHealthStashed << ", AmmoStashed: " << currentAmmoStashed;
}
