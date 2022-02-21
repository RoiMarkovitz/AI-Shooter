#include "Bullet.h"
#include <math.h>
#include "glut.h"

Bullet::Bullet(double xx, double yy, double angle, eTeam ownedByTeam, double bulletDamage)
{
	angleDirection = angle; // in radians
	dx = cos(angleDirection); 
	dy = sin(angleDirection);	
	x = xx + 0.5;
	y = yy + 0.5;
	isFired = false;
	ownerTeam = ownedByTeam;
	distancePassed = 0;
	damage = bulletDamage;
}

bool Bullet::move(const MapCell*const* map, const Player& attacker, bool isDummyShot)
{
	Player* playerInCell;
	MapMarks currentMark;
	if (isFired)
	{	
		playerInCell = map[(int)y][(int)x].getPlayerInCell();
		currentMark = map[(int)y][(int)x].getMark();
		//cout << "bullet distanced passed: " << distancePassed << endl;
		if (distancePassed > FIRE_BULLET_MAX_RANGE)
		{
			isFired = false;
			return false;
		}		

		if (playerInCell && !(attacker == *playerInCell))
		{
			if (playerInCell->getTeamNumber() == ownerTeam)
			{
				// then shot hit teamate			
				isFired = false;
				return false;
			}
			else if (playerInCell->getTeamNumber() != ownerTeam)
			{
				// then enemy is hit.
				isFired = false;
				//cout << "enemy was hit damage is: " << round(damage) << endl;
				if (!isDummyShot)
					playerInCell->takeDamage((int)round(damage));
				return true;
			}
		} // no player in the cell
															
		if (!playerInCell && currentMark != MapMarks::SPACE && currentMark != MapMarks::CORRIDOR)
		{
			// then shot was blocked by some object
			isFired = false;
			return false;
		}

		if (!isDummyShot)
			damage = MIN_BULLET_DAMAGE <= damage - DAMAGE_REDUCTION_RATE ? damage - DAMAGE_REDUCTION_RATE : MIN_BULLET_DAMAGE;
		// update x,y
		x += dx * BULLET_SPEED;
		y += dy * BULLET_SPEED;
		distancePassed += BULLET_SPEED;
	}
	return false;
}

void Bullet::show() const
{
	if(isFired)
	{ 
		if(ownerTeam == eTeam::Team1)
			glColor3d(0.54902, 0.547059, 0.847059); // pale blue
		else if (ownerTeam == eTeam::Team2)
			glColor3d(0.560784, 0.737255, 0.560784); // pale green
		else 
			glColor3d(0, 0, 0); // black
		glBegin(GL_POLYGON);
			glVertex2d(x, y + 0.3);
			glVertex2d(x+0.3, y);
			glVertex2d(x, y - 0.3);
			glVertex2d(x-0.3, y);
		glEnd();
	}
}

void Bullet::SimulateFire(MapCell** map, double damage)
{
	Player* playerInCell;
	MapMarks currentMark;
	MapCell* MapCell;
	while (isFired) 
	{
		MapCell = &map[(int)y][(int)x];
		playerInCell = MapCell->getPlayerInCell();
		currentMark = MapCell->getMark();

		if (distancePassed > FIRE_BULLET_MAX_RANGE)
		{
			isFired = false;
			return;
		}

		if(!playerInCell && currentMark != MapMarks::SPACE && currentMark != MapMarks::CORRIDOR)
		{
			isFired = false;
			return;		
		}

		MapCell->setRiskLevel(MapCell->getRiskLevel() + damage);
		
		// update x,y
		x += dx * BULLET_SPEED;
		y += dy * BULLET_SPEED;
		distancePassed += BULLET_SPEED;
		
	}
}

bool Bullet::isEnemyHitableByBullet(const MapCell*const* map, const Player& attacker) 
{
	while (isFired)
	{
		if (move(map, attacker, true))
			return true;
	} // while

	return false;
}

bool Bullet::isOuterMazeWallInPath(const MapCell*const* map, double fireDistance)
{
	MapMarks currentMark;
	while (isFired)
	{
		currentMark = map[(int)y][(int)x].getMark();
		if (distancePassed > fireDistance)
		{
			isFired = false;
			return false;
		}

		if (currentMark == MapMarks::OUTER_WALL)
		{
			isFired = false;
			return true;
		}
			
		// update x,y
		x += dx * BULLET_SPEED;
		y += dy * BULLET_SPEED;

		distancePassed += BULLET_SPEED;
	} // while

	return false;
}