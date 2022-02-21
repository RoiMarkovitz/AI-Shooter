#include "HealthStorage.h"

HealthStorage::HealthStorage(int row, int col) : Storage(row, col)
{
	currentHealthStored = MAX_HEALTH_IN_STORAGE;
}

void HealthStorage::decreaseHealth(int health)
{
	int tempCurrentHealthStored = currentHealthStored - health;
	if (tempCurrentHealthStored > MIN_HEALTH_IN_STORAGE)
		currentHealthStored = tempCurrentHealthStored;
	else
	{
		currentHealthStored = MIN_HEALTH_IN_STORAGE;
		isEmpty = true;
	}	
}