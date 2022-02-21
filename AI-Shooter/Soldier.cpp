#include "Soldier.h"

Soldier::Soldier(int row, int col, eTeam teamNum, int roomNum, Support* mySupport) : Player(row, col, teamNum, roomNum)
{
	currentAmmo = MAX_AMMO_SOLDIER;
	bullet = nullptr;
	grenade = nullptr;
	support = mySupport;
	criticalAmmoLevel = (rand() % (MAX_CRITICAL_AMMO_LEVEL - MIN_CRITICAL_AMMO_LEVEL + 1)) + MIN_CRITICAL_AMMO_LEVEL;
	countTurnsStopEscape = 0;
}

Soldier::~Soldier()
{
	delete bullet;
	delete grenade;
}

void Soldier::playOneTurn(const vector<Player*>& enemyTeam, const MapCell*const* map)
{
	if (!isDead)
	{
		if (scanAreaForEnemyGrenades(enemyTeam, map))
			moveToSafestPosition(SAFE_SQUARE_SCAN_SIZE, SAFE_SQUARE_SCAN_SIZE, currentPoint2D.getRow(), currentPoint2D.getCol(), map);
		else if (!survive(enemyTeam, support, map))
			war(enemyTeam, map);
	}
}

bool Soldier::survive(const vector<Player*>& enemyTeam, const Support* support, const MapCell*const* map)
{
	double distanceToSupport;
	if (criticalHealthLevel >= currentHealth || criticalAmmoLevel >= currentAmmo)
	{
		cout << *this << ", SURVIVAL Mode" << endl;
		if (support) // support team member is alive
		{
			distanceToSupport = currentPoint2D.calcEuclideanDistance(support->getCurrentPoint2D());
			if (distanceToSupport > support->getMaxSupplyRange())
				moveToPosition(support->getCurrentPoint2D(), map, eAstar::ShortestPath);
			else
				war(enemyTeam, map);
		}	
		else if (rand() % 100 >= CHANCE_TO_STOP_ESCAPING && countTurnsStopEscape == 0 || currentAmmo == MIN_AMMO_SOLDIER)
			moveToSafestPosition(SAFE_SQUARE_SCAN_SIZE, SAFE_SQUARE_SCAN_SIZE, currentPoint2D.getRow(), currentPoint2D.getCol(), map);
		else
		{
			countTurnsStopEscape++;
			countTurnsStopEscape = countTurnsStopEscape == MAX_TURNS_TO_STOP_ESCAPE ? 0 : countTurnsStopEscape;
			war(enemyTeam, map);
		}
								
		return true;
	}
	else // not in survival mode
	{
		countTurnsStopEscape = 0;
		return false;
	}
		
}

bool Soldier::war(const vector<Player*>& enemyTeam, const MapCell*const* map)
{
	cout << *this << ", WAR Mode" << endl;
	int attemptToShootChance;
	const Player* enemy = findEnemy(enemyTeam, map);
	if (enemy)
	{
		attemptToShootChance = (rand() % (MAX_ATTEMPT_TO_SHOOT_CHANCE - MIN_ATTEMPT_TO_SHOOT_CHANCE + 1)) + MIN_ATTEMPT_TO_SHOOT_CHANCE;
		if (rand() % 100 < attemptToShootChance) // chance to try and shoot first
		{
			if (shoot(*enemy, map), bullet); // try to shoot
			else if (throwGrenade(*enemy, enemyTeam, map), grenade); // try to throw grenade
			else
				moveToPosition(enemy->getCurrentPoint2D(), map, eAstar::ShortestPath);
		}
		else // chance to try and throw grenade first
		{
			if (throwGrenade(*enemy, enemyTeam, map), grenade); // try to throw grenade
			else if (shoot(*enemy, map), bullet); // try to shoot
			else
				moveToPosition(enemy->getCurrentPoint2D(), map, eAstar::ShortestPath);
		}
		return true; // enemy found
	}
	else
		return false; // no enemy found
}

const Player* Soldier::findEnemy(const vector<Player*>& enemyTeam, const MapCell*const* map) const
{
	vector<Player*> enemiesInRange;
	const Player* currentEnemy;
	enemiesInRange = getEnemiesInRange(map);
	if (enemiesInRange.empty()) // no enemies in range
		currentEnemy = findNearestPlayerInMap(enemyTeam, map);
	else // enemies in range
	{
		currentEnemy = findNearestVisibleEnemyInRange(enemiesInRange, map);
		if (!currentEnemy) // no visible enemy in range
			currentEnemy = findNearestPlayerInMap(enemiesInRange, map);
	}

	return currentEnemy;
}

bool Soldier::shoot(const Player& enemy, const MapCell*const* map)
{										// bullet does not exist
	if (isShootingAllowed(enemy, map) && !bullet)
	{		
		bullet = new Bullet(currentPoint2D.getCol(), currentPoint2D.getRow(), currentPoint2D.calculateAngleBetweenTwoPoints(enemy.getCurrentPoint2D()), teamNumber);
		bullet->setIsFired(true);	
		decreaseAmmo(1);	
		return true; 
	}
	return false;
}

void Soldier::moveBullet(const MapCell*const* map)
{
	// bullet exists but hit something already
	if (bullet && !bullet->getIsFired())
	{		
		delete bullet; 
		bullet = nullptr;
	}

	// bullet is active and still moving
	if (bullet && bullet->getIsFired())	
		bullet->move(map, *this, false);	
}

bool Soldier::isShootingAllowed(const Player& enemy, const MapCell*const* map) const
{
	Bullet dummyBullet = Bullet(currentPoint2D.getCol(), currentPoint2D.getRow(), currentPoint2D.calculateAngleBetweenTwoPoints(enemy.getCurrentPoint2D()), teamNumber);
	dummyBullet.setIsFired(true);
	// 1. check if there is enough ammo
	// 2. check if enemy in range
	// 3. check if enemy visible - shoot transparent bullet to see if it can hit enemy (no obstacle on the way)
	return currentAmmo > MIN_AMMO_SOLDIER
		&& currentPoint2D.calcEuclideanDistance(enemy.getCurrentPoint2D()) <= FIRE_BULLET_MAX_RANGE
		&& dummyBullet.isEnemyHitableByBullet(map, *this);
}

bool Soldier::throwGrenade(const Player& targetEnemy, const vector<Player*>& enemyTeam, const MapCell*const* map)
{
	if (isThrowingGrenadeAllowed(targetEnemy, enemyTeam, map) && !grenade)
	{
		Point2D targetPos = targetEnemy.getCurrentPoint2D();
		grenade = new Grenade(currentPoint2D.getCol(), currentPoint2D.getRow(), targetPos, currentPoint2D.calculateAngleBetweenTwoPoints(targetEnemy.getCurrentPoint2D()), teamNumber);
		decreaseAmmo(GRENADE_AMMO_COST);
		return true;
	}
	return false;
}

void Soldier::moveGrenade(const MapCell*const* map)
{
	//grenade exists, is detonated and ALL particles hit something already
	if (grenade && grenade->getDidAllParticlesHitSomething())
	{	
		delete grenade; 
		grenade = nullptr;
	}

	// grenade exists, is detonated and NOT ALL particles hit something already
	if (grenade && grenade->getIsDetonated())	
		grenade->moveGrenadeParticles(map, *this);	

	// grenade exists and is still moving to target 
	if (grenade && !grenade->getIsDetonated())	
		grenade->moveGrenadeToTarget(map);
}

bool Soldier::isThrowingGrenadeAllowed(const Player& targetEnemy, const vector<Player*>& enemyTeam, const MapCell*const* map) const
{	
	int numberOfHitableEnemies = 0;
	double distanceToTargetEnemy;
	bool outerMazeWallInPath;
	vector<Player*>::const_iterator itr = enemyTeam.begin();
	vector<Player*>::const_iterator itrEnd = enemyTeam.end();

	distanceToTargetEnemy = currentPoint2D.calcEuclideanDistance(targetEnemy.getCurrentPoint2D());
	Bullet dummyBullet = Bullet(currentPoint2D.getCol(), currentPoint2D.getRow(), currentPoint2D.calculateAngleBetweenTwoPoints(targetEnemy.getCurrentPoint2D()), teamNumber);
	dummyBullet.setIsFired(true);
	outerMazeWallInPath = dummyBullet.isOuterMazeWallInPath(map, distanceToTargetEnemy);

	for (; itr != itrEnd; ++itr)
	{
		if (!(*(*itr) == targetEnemy))
		{
			// make targetEnemy shoot dummy bullets towards his teammates.
			// see how many enemies are hitable from his spot to know the potential of the grenade thrown
			dummyBullet = Bullet(targetEnemy.getCurrentPoint2D().getCol(), targetEnemy.getCurrentPoint2D().getRow(), targetEnemy.getCurrentPoint2D().calculateAngleBetweenTwoPoints((*itr)->getCurrentPoint2D()), teamNumber);
			dummyBullet.setIsFired(true);
			numberOfHitableEnemies += dummyBullet.isEnemyHitableByBullet(map, targetEnemy);
		}
		else 
			numberOfHitableEnemies++;	
	}
	
	dummyBullet = Bullet(currentPoint2D.getCol(), currentPoint2D.getRow(), currentPoint2D.calculateAngleBetweenTwoPoints(targetEnemy.getCurrentPoint2D()), teamNumber);
	dummyBullet.setIsFired(true);

	// 1. check if there is enough ammo
	// 2. check if enemy in range
	// 3. check if enemy is NOT visible - shoot transparent bullet to see if it can hit enemy (no obstacle on the way)
	// 3. grenade hit location has potential to hit more than one player
	// 4. no outer map wall (black) in the path
	return currentAmmo - GRENADE_AMMO_COST >= MIN_AMMO_SOLDIER
		&& (distanceToTargetEnemy <= THROW_GRENADE_MAX_RANGE && distanceToTargetEnemy >= THROW_GRENADE_MIN_RANGE)
		&& (!dummyBullet.isEnemyHitableByBullet(map, *this) || numberOfHitableEnemies > 1)
		&& !outerMazeWallInPath;
}

void Soldier::increaseAmmo(int ammo)
{
	int tempCurrentAmmo = currentAmmo + ammo;
	if (tempCurrentAmmo > MAX_AMMO_SOLDIER)
		currentAmmo = MAX_AMMO_SOLDIER;
	else
		currentAmmo = tempCurrentAmmo;
}

void Soldier::decreaseAmmo(int ammo)
{
	int tempCurrentAmmo = currentAmmo - ammo;
	if (tempCurrentAmmo > MIN_AMMO_SOLDIER)
		currentAmmo = tempCurrentAmmo;
	else
		currentAmmo = MIN_AMMO_SOLDIER;
}

const Player* Soldier::findNearestVisibleEnemyInRange(const vector<Player*>& enemyTeamInRange, const MapCell*const* map) const
{
	double minDistanceFromVisibleEnemy{};
	double distanceFromVisibleEnemy;
	bool firstFound = false;
	Player* nearestVisibleEnemy{};
	Cell* pCell;
	vector<Player*>::const_iterator itr = enemyTeamInRange.begin();
	vector<Player*>::const_iterator itrEnd = enemyTeamInRange.end();

	if (!enemyTeamInRange.empty())
	{
		for (; itr != itrEnd; ++itr)
		{
			if (!(*itr)->getIsDead())
			{
				if (isShootingAllowed(*(*itr), map))
				{
					pCell = Astar::aStar(currentPoint2D, (*itr)->getCurrentPoint2D(), map, eAstar::ShortestPath);
					if (pCell)
					{
						distanceFromVisibleEnemy = pCell->getF();
						Astar::freePathMemory(pCell);
						// get initial min distance
						if (!firstFound)
						{
							firstFound = true;
							minDistanceFromVisibleEnemy = distanceFromVisibleEnemy;
							nearestVisibleEnemy = (*itr);
						}
						if (distanceFromVisibleEnemy < minDistanceFromVisibleEnemy)
						{
							minDistanceFromVisibleEnemy = distanceFromVisibleEnemy;
							nearestVisibleEnemy = (*itr);
						}
					}		
				}
			}
		}
		return nearestVisibleEnemy;
	}

	return nullptr;
}

void Soldier::toOs(ostream& os) const
{
	os << ", Ammo: " << currentAmmo;
}
