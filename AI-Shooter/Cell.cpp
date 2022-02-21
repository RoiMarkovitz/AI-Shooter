#include "Cell.h"
#include <math.h>

Cell::Cell(const Cell& other) {
	position = other.position;
	targetPosition = other.targetPosition;
	parent = other.parent;
	g = other.g;
	h = other.h;
	f = h + g;
}

Cell::Cell(int r, int c, int tr, int tc, double g, Cell* p)
{
	position = Point2D(r, c);
	targetPosition = Point2D(tr, tc);
	parent = p;
	this->g = g;
	h = sqrt(pow((double)r - tr, 2) + pow((double)c - tc, 2));
	f = h + g;
}




