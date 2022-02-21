#ifndef __STORAGE_H
#define __STORAGE_H

#include "Point2D.h"

class Storage
{
protected:
	Point2D point2D;
	bool isEmpty;

public:
	Storage() = delete;
	Storage(int row, int col);
	virtual ~Storage() = 0;
	const Point2D& getPoint2D() const { return point2D; }
	bool getIsEmpty() const { return isEmpty; }
};

#endif // __STORAGE_H