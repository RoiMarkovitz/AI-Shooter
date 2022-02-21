#include "Player.h"
#include "Soldier.h"

int Player::idCounter = 0;

Player::Player(int row, int col, eTeam teamNum, int roomNum) : id(++idCounter)
{
	currentHealth = MAX_PLAYER_HEALTH;
	isDead = false;
	hasSteppedInPath = false;
	currentPoint2D = Point2D(row, col);
	beforePoint2D = Point2D(row, col);
	teamNumber = teamNum;
	roomId = roomNum;
	criticalHealthLevel = (rand() % (MAX_CRITICAL_HEALTH_LEVEL - MIN_CRITICAL_HEALTH_LEVEL + 1)) + MIN_CRITICAL_HEALTH_LEVEL;	
}

Player::~Player()
{	
}

void Player::takeDamage(int damage)
{
	int tempCurrentHealth = currentHealth - damage;
	if (tempCurrentHealth < MIN_PLAYER_HEALTH)
	{
		currentHealth = MIN_PLAYER_HEALTH;
		isDead = true;
	}
	else
		currentHealth = tempCurrentHealth;
}

void Player::increaseHealth(int health)
{
	int tempCurrentHealth = currentHealth + health;
	if (tempCurrentHealth > MAX_PLAYER_HEALTH)
		currentHealth = MAX_PLAYER_HEALTH;
	else
		currentHealth = tempCurrentHealth;
}

void Player::moveToPosition(const Point2D& targetPoint, const MapCell*const* map, eAstar option)
{
	// Player should not move, startPoint and targetPoint are the same
	if (currentPoint2D == targetPoint)
		return;

	Cell* pCell = Astar::aStar(currentPoint2D, targetPoint, map, option);
	if (!pCell)
		return;

	if (pCell->getParent()->getParent() == nullptr) // then next step is target
	{
		if (map[pCell->getPosition().getRow()][pCell->getPosition().getCol()].getMark() == MapMarks::SPACE
			|| map[pCell->getPosition().getRow()][pCell->getPosition().getCol()].getMark() == MapMarks::CORRIDOR)
		{
			currentPoint2D.setPoint2D(pCell->getPosition().getRow(), pCell->getPosition().getCol());
			delete pCell->getParent(); 
			delete pCell; 
		}		
		else
			return; // dont want to step over target, as it isnt space or corridor
	}
	else
	{   
		Cell* memory;
		while (pCell->getParent() != nullptr)
		{
			
			if (pCell->getParent()->getParent() == nullptr) // next step
			{
				currentPoint2D.setPoint2D(pCell->getPosition().getRow(), pCell->getPosition().getCol());
				delete pCell->getParent(); 
				delete pCell; 
				return;
			}
			memory = pCell;
			pCell = pCell->getParent();
			delete memory; 
		}
	}
}

const vector<Player*> Player::getEnemiesInRange(const MapCell*const* map) const
{
	int playerRow = currentPoint2D.getRow();
	int playerCol = currentPoint2D.getCol();
	Player* playerInCell;
	vector<Player*> enemiesInRange;
	
	// check if enemies are in range of the player and add them to vector
	for (int r = playerRow - SCAN_ENEMIES_SQUARE_SIZE / 2; r <= playerRow + SCAN_ENEMIES_SQUARE_SIZE / 2; r++)
		for (int c = playerCol - SCAN_ENEMIES_SQUARE_SIZE / 2; c <= playerCol + SCAN_ENEMIES_SQUARE_SIZE / 2; c++)
		{
			if (r >= 0 && r < MSZ && c >= 0 && c < MSZ)
			{
				playerInCell = map[r][c].getPlayerInCell();
				if (playerInCell && !isSameTeam(*playerInCell))
					enemiesInRange.push_back(playerInCell);
			}		
		}
	
	return enemiesInRange; 
}

Player* Player::findNearestPlayerInMap(const vector<Player*>& team, const MapCell*const* map) const
{
	double minDistanceFromPlayer{};
	double distanceFromPlayer;
	Player* nearestPlayer{};
	Cell* pCell;
	vector<Player*>::const_iterator itr = team.begin();
	vector<Player*>::const_iterator itrEnd = team.end();
	
	if (!team.empty())
	{
		pCell = Astar::aStar(currentPoint2D, (*itr)->currentPoint2D, map, eAstar::ShortestPath);

		// ignore unreachable and dead but still not erased players and if its the same player 
		while ((*itr)->getIsDead() || !pCell || *this == (*(*itr)))
		{
			Astar::freePathMemory(pCell);
			++itr;
			if (itr == itrEnd)
				return nullptr;		
			pCell = Astar::aStar(currentPoint2D, (*itr)->currentPoint2D, map, eAstar::ShortestPath);
		}

		// get initial min distance
		if (itr != itrEnd)
		{
			minDistanceFromPlayer = pCell->getF();
			Astar::freePathMemory(pCell);
			nearestPlayer = (*itr);
			++itr;		
		}

		// see if there are any nearer players
		for (; itr != itrEnd; ++itr)
		{
			pCell = Astar::aStar(currentPoint2D, (*itr)->getCurrentPoint2D(), map, eAstar::ShortestPath);
			if (!(*itr)->getIsDead() && pCell && !(*this == (*(*itr))))
			{
				distanceFromPlayer = pCell->getF();
				Astar::freePathMemory(pCell);
				if (distanceFromPlayer < minDistanceFromPlayer)
				{
					minDistanceFromPlayer = distanceFromPlayer;
					nearestPlayer = (*itr);
				}			
			}
		}
	}
	
	return nearestPlayer;
}

void Player::moveToSafestPosition(int width, int height, int targetRow, int targetCol, const MapCell*const* map)
{
	Point2D safestPoisition;
	double minValue;
	bool foundFirst = false;
	int row{}, col{};
	MapMarks currentMark;
	int playerRow, playerCol;
	playerRow = currentPoint2D.getRow();
	playerCol = currentPoint2D.getCol();

	for (int r = targetRow - height / 2; r <= targetRow + height / 2; r++)
	{
		for (int c = targetCol - width / 2; c <= targetCol + width / 2; c++)
		{
			if (r >= 0 && r < MSZ && c >= 0 && c < MSZ)
			{
				currentMark = map[r][c].getMark();
				if ((currentMark == MapMarks::SPACE || currentMark == MapMarks::CORRIDOR || (r == playerRow && c == playerCol)) && !foundFirst)
				{
					minValue = map[r][c].getRiskLevel();
					row = r; 
					col = c;
					foundFirst = true;
				}
				else if ((currentMark == MapMarks::SPACE || currentMark == MapMarks::CORRIDOR || (r == playerRow && c == playerCol)) && foundFirst && map[r][c].getRiskLevel() < minValue)
				{
					minValue = map[r][c].getRiskLevel();
					row = r;
					col = c;
				}
			}	
		}
	}
	if (foundFirst)
	{
		safestPoisition = Point2D(row, col);
		moveToPosition(safestPoisition, map, eAstar::ShortestPath);
	}		
	else
		moveToPosition(currentPoint2D, map, eAstar::ShortestPath);		
}

bool Player::scanAreaForEnemyGrenades(const vector<Player*>& enemyTeam, const MapCell*const* map) const
{
	vector<Player*>::const_iterator itr = enemyTeam.begin();
	vector<Player*>::const_iterator itrEnd = enemyTeam.end();
	Soldier* tempSoldier;
	Grenade* grenade;
	int playerRow = currentPoint2D.getRow();
	int playerCol = currentPoint2D.getCol();

	for (; itr != itrEnd; ++itr)
	{
		tempSoldier = dynamic_cast<Soldier*>((*itr));
		if (tempSoldier && tempSoldier->getGrenade())
			grenade = tempSoldier->getGrenade();
		else
			continue;
		for (int r = playerRow - SCAN_GRENADE_AREA_SIZE / 2; r <= playerRow + SCAN_GRENADE_AREA_SIZE / 2; r++)
			for (int c = playerCol - SCAN_GRENADE_AREA_SIZE / 2; c <= playerCol + SCAN_GRENADE_AREA_SIZE / 2; c++)
			{
				if (r >= 0 && r < MSZ && c >= 0 && c < MSZ)
				{
					if ((int)grenade->getY() == r && (int)grenade->getX() == c && !grenade->getIsDetonated())
					{
						cout << *this << ", EVADE GRENADE Mode" << endl;
						return true;
					}
				}									
			}
	}
	
	return false;
}

ostream& operator<<(ostream& os, const Player& p)
{
	os << typeid(p).name() + 5 << ", " << (p.teamNumber == eTeam::Team1 ? "Blue" : "Green") << " Team" ", ID: " << p.id << ", Health: " << p.currentHealth;
	p.toOs(os);
	return os;
}


