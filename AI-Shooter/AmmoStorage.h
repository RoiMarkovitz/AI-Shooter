#ifndef __AMMO_STORAGE_H
#define __AMMO_STORAGE_H

#include "Storage.h"

int const MAX_AMMO_IN_STORAGE = 16;
int const MIN_AMMO_IN_STORAGE = 0;

class AmmoStorage : public Storage
{
private:
	int currentAmmoStored;

public:
	AmmoStorage() = delete;
	AmmoStorage(int row, int col);
	int getAmmoInStorage() const { return currentAmmoStored; }
	void decreaseAmmo(int ammo);
};

#endif // __AMMO_STORAGE_H