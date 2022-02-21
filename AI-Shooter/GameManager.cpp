#include "GameManager.h"

GameManager::GameManager()
{
	CreateMap::createMap(&map, team1, team2, healthStorages, ammoStorages);
	hasGameStarted = false;
	hasGameEnded = false;
}

GameManager::~GameManager()
{
	for (int i = 0; i < MSZ; i++)
		delete []map[i];
	delete []map;
	
	vector<Player*>::iterator itrTeam1 = team1.begin();
	vector<Player*>::iterator itrTeam1End = team1.end();
	for (; itrTeam1 != itrTeam1End; ++itrTeam1)
		delete *itrTeam1;

	vector<Player*>::iterator itrTeam2 = team2.begin();
	vector<Player*>::iterator itrTeam2End = team2.end();
	for (; itrTeam2 != itrTeam2End; ++itrTeam2)
		delete *itrTeam2;
	
	vector<HealthStorage*>::iterator itrHealthStorages = healthStorages.begin();
	vector<HealthStorage*>::iterator itrHealthStoragesEnd = healthStorages.end();
	for (; itrHealthStorages != itrHealthStoragesEnd; ++itrHealthStorages)
		delete *itrHealthStorages;
	
	vector<AmmoStorage*>::iterator itrAmmoStorages = ammoStorages.begin();
	vector<AmmoStorage*>::iterator itrAmmoStoragesEnd = ammoStorages.end();
	for (; itrAmmoStorages != itrAmmoStoragesEnd; ++itrAmmoStorages)
		delete *itrAmmoStorages;
}

void GameManager::playOneTurn()
{		
	playTeamTurn(team1, team2);
	playTeamTurn(team2, team1);
	updateLocationsInMap();
	checkIfGameHasEnded();
}

void GameManager::playTeamTurn(const vector<Player*>& team, const vector<Player*>& enemyTeam)
{
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	Player* tempPlayer;

	for (; itr != itrEnd; ++itr)
	{
		tempPlayer = (*itr);
		updateRiskLevelMap(*tempPlayer);
		if (dynamic_cast<Soldier*>(tempPlayer))
			((Soldier*)tempPlayer)->playOneTurn(enemyTeam, map);
		else
		{
			((Support*)tempPlayer)->playOneTurn(team, enemyTeam, healthStorages, ammoStorages, map);
			updateStoragesCondition(healthStorages);  
			updateStoragesCondition(ammoStorages);
		}			
		clearRiskLevelMap();
	}		
}

void GameManager::updateLocationsInMap()
{
	updateTeamProjectiles(team1);
	updateTeamProjectiles(team2);
	updateTeamStatus(team1);
	updateTeamStatus(team2);
	updateTeamLocations(team1);
	updateTeamLocations(team2);
}

void GameManager::updateTeamProjectiles(const vector<Player*>& team) const
{
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	Soldier* tempSoldier;

	for (; itr != itrEnd; ++itr)
	{
		tempSoldier = dynamic_cast<Soldier*>((*itr));
		if (tempSoldier)
		{
			tempSoldier->moveBullet(map);
			tempSoldier->moveGrenade(map);
		}
	}
}

void GameManager::updateTeamStatus(vector<Player*>& team)
{
	vector<Player*>::iterator itr = team.begin();
	Player* tempPlayer, *playerInCell;
	Soldier* tempSoldier;
	MapCell* mapBeforeCell;
	bool wasPlayerDeleted = false;
	while (itr != team.end())
	{
		tempPlayer = (*itr);
		mapBeforeCell = &map[tempPlayer->getBeforePoint2D().getRow()][tempPlayer->getBeforePoint2D().getCol()];
		if (tempPlayer->getIsDead())
		{
			playerInCell = mapBeforeCell->getPlayerInCell();
			// player in the cell with the same id as the one being removed
			// so if happens only once 
			if (playerInCell && (*playerInCell == (*tempPlayer)))
			{
				if(mapBeforeCell->getRoomId() == CORRIDOR_ID)
					mapBeforeCell->setMark(MapMarks::CORRIDOR);
				else
					mapBeforeCell->setMark(MapMarks::SPACE);
					
				mapBeforeCell->setPlayerInCell(nullptr);
			}

			tempSoldier = dynamic_cast<Soldier*>(tempPlayer);
			// its support or soldier that has no active bullet or grenade
			if (!tempSoldier || (tempSoldier && !tempSoldier->getBullet() && !tempSoldier->getGrenade()))
			{
				itr = team.erase(itr);
				wasPlayerDeleted = true;
			}
		}

		if (!wasPlayerDeleted)
			++itr;
			
		wasPlayerDeleted = false;
	}
}

// after all players made their moves, update map accordingly
void GameManager::updateTeamLocations(const vector<Player*>& team) const
{	
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	Player* tempPlayer;
	int currentRow, currentCol;
	int beforeRow, beforeCol;
	MapCell* mapCurrentCell, *mapBeforeCell;
	eTeam teamNum;
	MapMarks mark;
	for (; itr != itrEnd; ++itr)
	{
		tempPlayer = (*itr);
		if (!tempPlayer->getIsDead())
		{	
			teamNum = tempPlayer->getTeamNumber();
			currentRow = tempPlayer->getCurrentPoint2D().getRow();
			currentCol = tempPlayer->getCurrentPoint2D().getCol();
			beforeRow = tempPlayer->getBeforePoint2D().getRow();
			beforeCol = tempPlayer->getBeforePoint2D().getCol();
			mapCurrentCell = &map[currentRow][currentCol];
			mapBeforeCell = &map[beforeRow][beforeCol];

			// not allow players to step on one another 
			if (mapCurrentCell->getPlayerInCell())
			{
				tempPlayer->setCurrentPoint2D(beforeRow, beforeCol);
				continue; 
			}

			if (tempPlayer->getHasSteppedInPath())
				mapBeforeCell->setMark(MapMarks::CORRIDOR);
			else
				mapBeforeCell->setMark(MapMarks::SPACE);

			if (mapCurrentCell->getMark() == MapMarks::CORRIDOR)
				tempPlayer->setHasSeteppedInPath(true);
			else
				tempPlayer->setHasSeteppedInPath(false);

			mapBeforeCell->setPlayerInCell(nullptr);

			if (dynamic_cast<Soldier*>(tempPlayer))
			{
				mark = teamNum == eTeam::Team1 ? MapMarks::TEAM_ONE_SOLDIER : MapMarks::TEAM_TWO_SOLDIER;
				mapCurrentCell->setMark(mark);
			}		
			else if (dynamic_cast<Support*>(tempPlayer))
			{
				mark = teamNum == eTeam::Team1 ? MapMarks::TEAM_ONE_SUPPORT : MapMarks::TEAM_TWO_SUPPORT;
				mapCurrentCell->setMark(mark);
			}

			mapCurrentCell->setPlayerInCell(tempPlayer);
		
			tempPlayer->setRoomId(mapCurrentCell->getRoomId());
			tempPlayer->setBeforePoint2D(currentRow, currentCol);
		}
	}	
}

void GameManager::updateRiskLevelMap(const Player& player)  
{
	Soldier* tempSoldier;
	Grenade* tempSoldierGrenade;
	vector<Player*> enemiesInRange;
	enemiesInRange = player.getEnemiesInRange(map);
	vector<Player*>::const_iterator itr = enemiesInRange.begin();
	vector<Player*>::const_iterator itrEnd = enemiesInRange.end();

	for (; itr != itrEnd; ++itr)
	{
		tempSoldier = dynamic_cast<Soldier*>((*itr));
		if (tempSoldier)
		{
			Grenade dummyGrenade1 = Grenade(tempSoldier->getCurrentPoint2D().getCol(), tempSoldier->getCurrentPoint2D().getRow(), 360);
			dummyGrenade1.SimulateExplosion(map, SIMULATION_DAMAGE);
			centerPositionsMapCellsToClean.push_back(&tempSoldier->getCurrentPoint2D());
			tempSoldierGrenade = tempSoldier->getGrenade();
			if (tempSoldierGrenade)
			{
				Grenade dummyGrenade2 = *tempSoldierGrenade;
				dummyGrenade2.SimulateExplosion(map, SIMULATION_DAMAGE);
				centerPositionsMapCellsToClean.push_back(&tempSoldierGrenade->getTargetPosition());
			}
		}
	}
}

void GameManager::clearRiskLevelMap()
{
	vector<Point2D*>::iterator itr = centerPositionsMapCellsToClean.begin();
	vector<Point2D*>::iterator itrEnd = centerPositionsMapCellsToClean.end();
	int clearSquareSize = (int)(FIRE_BULLET_MAX_RANGE * 2 + 1);

	for (; itr != itrEnd; ++itr)
		for (int r = (*itr)->getRow() - clearSquareSize / 2; r <= (*itr)->getRow() + clearSquareSize / 2; r++)
			for (int c = (*itr)->getCol() - clearSquareSize / 2; c <= (*itr)->getCol() + clearSquareSize / 2; c++)
				if (r >= 0 && r < MSZ && c >= 0 && c < MSZ)
					map[r][c].setRiskLevel(MIN_RISK_LEVEL);

	centerPositionsMapCellsToClean.clear();
}

void GameManager::checkIfGameHasEnded() 
{
	bool isAllTeam1Dead = isAllTeamDead(team1);
	bool isAllTeam2Dead = isAllTeamDead(team2);

	if ((isAllTeam1Dead && isAllTeam2Dead) || (noMoreAmmoForTeam(team1) && noMoreAmmoForTeam(team2))
		|| (isOnlySupportLeftInTeam(team1) && isOnlySupportLeftInTeam(team2)))
	{
		cout << "GAME ENDED - TIE!" << endl;
		hasGameEnded = true;
	}	
	else if (isAllTeam1Dead && !isAllTeam2Dead || isOnlySupportLeftInTeam(team1) && noActivePorjectilesForTeam(team1))
	{
		cout << "GAME ENDED - TEAM TWO WON!" << endl;
		hasGameEnded = true;
	}		
	else if (!isAllTeam1Dead && isAllTeam2Dead || isOnlySupportLeftInTeam(team2) && noActivePorjectilesForTeam(team2))
	{
		cout << "GAME ENDED - TEAM ONE WON!" << endl;
		hasGameEnded = true;
	}		
}

bool GameManager::isAllTeamDead(const vector<Player*>& team) const
{
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	for (; itr != itrEnd; ++itr)
		if (!(*itr)->getIsDead())
			return false;

	return true;
}

bool GameManager::isOnlySupportLeftInTeam(const vector<Player*>& team) const
{
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	for (; itr != itrEnd; ++itr)
		if (dynamic_cast<Soldier*>((*itr)))
			return false;

	return true;
}

bool GameManager::noActivePorjectilesForTeam(const vector<Player*>& team) const
{
	Soldier* tempSoldier;
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	for (; itr != itrEnd; ++itr)
	{
		tempSoldier = dynamic_cast<Soldier*>((*itr));
		if (tempSoldier)
		{
			if (tempSoldier->getBullet() || tempSoldier->getGrenade())
				return false;
		}
	}
	return true;
}

bool GameManager::noMoreAmmoForTeam(const vector<Player*>& team) const
{
	Player* tempPlayer;
	int numberOfTeamMembersWithNoAmmo = 0;
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	for (; itr != itrEnd; ++itr)
	{
		tempPlayer = (*itr);
		if (dynamic_cast<Soldier*>(tempPlayer))
			numberOfTeamMembersWithNoAmmo += ((Soldier*)tempPlayer)->getCurrentAmmo() == 0;	
		else
			numberOfTeamMembersWithNoAmmo += ((Support*)tempPlayer)->getCurrentAmmoStashed() == 0 && ammoStorages.empty();
	}

	if (!noActivePorjectilesForTeam(team))
		return false;

	return numberOfTeamMembersWithNoAmmo == team.size();
}
