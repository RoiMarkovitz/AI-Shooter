#include "AmmoStorage.h"

AmmoStorage::AmmoStorage(int row, int col) : Storage(row, col)
{
	currentAmmoStored = MAX_AMMO_IN_STORAGE;
}

void AmmoStorage::decreaseAmmo(int ammo)
{
	int tempCurrentAmmoStored = currentAmmoStored - ammo;
	if (tempCurrentAmmoStored > MIN_AMMO_IN_STORAGE)
		currentAmmoStored = tempCurrentAmmoStored;
	else
	{
		currentAmmoStored = MIN_AMMO_IN_STORAGE;
		isEmpty = true;
	}
}