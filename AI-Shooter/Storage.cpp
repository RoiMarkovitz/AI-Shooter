#include "Storage.h"

Storage::Storage(int row, int col)
{
	point2D = Point2D(row, col);
	isEmpty = false;
}

Storage::~Storage()
{	
}