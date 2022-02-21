#ifndef __CELL_H
#define __CELL_H

#include "Definitions.h"
#include "Point2D.h"

class Cell
{
private:
	Point2D position;
	Point2D targetPosition;	
	Cell* parent;
	double h, g, f;

	Cell() = default;
public:
	
	Cell(const Cell& other);
	Cell(int r, int c, int tr, int tc, double g, Cell* p);

	Point2D& getPosition(){ return position; }
	const Point2D& getPosition() const { return position; }
	Point2D& getTargetPosition() { return targetPosition; }
	const Point2D& getTargetPosition() const { return targetPosition; }
	Cell* getParent() const { return parent; }
	double getF() const { return f; }
	double getG() const { return g; }
	void setG(double newG) { g = newG; }
	void setF(double newF) { f = newF; }
	
	bool operator == (const Cell& other) const { return position == other.position; }

	friend class Astar;
};

#endif // __CELL_H
