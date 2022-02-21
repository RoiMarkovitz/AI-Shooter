#ifndef __HEALTH_STORAGE_H
#define __HEALTH_STORAGE_H

#include "Storage.h"

int const MAX_HEALTH_IN_STORAGE = 100;
int const MIN_HEALTH_IN_STORAGE = 0;

class HealthStorage : public Storage
{
private:
	int currentHealthStored;

public:
	HealthStorage() = delete;
	HealthStorage(int row, int col);
	int getHealthInStorage() const { return currentHealthStored; }
	void decreaseHealth(int health);
};

#endif // __HEALTH_STORAGE_H