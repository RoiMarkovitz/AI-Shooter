#ifndef __POINT2D_H
#define __POINT2D_H

#include "math.h"

class Point2D
{
private:
	int row;
	int col;

	Point2D();

public:
	Point2D(int r, int c); 
	void setPoint2D(int r, int c) { row = r; col = c; }
	int getRow() const { return row; }
	int getCol() const { return col; }
	double calcEuclideanDistance(const Point2D& other) const { return sqrt(pow((double)row - other.row, 2) + pow((double)col - other.col, 2)); }
	double calculateAngleBetweenTwoPoints(const Point2D& other) const { return atan2((double)other.row - (double)row, (double)other.col - (double)col); }
	bool operator == (const Point2D& other) const { return row == other.row && col == other.col; }
	
	friend class Room;
	friend class Player;
	friend class Storage;
	friend class Grenade;
	friend class MapCell;
	friend class Cell;
};

#endif // __POINT2D_H