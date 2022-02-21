#ifndef __COMPARE_CELLS_H
#define __COMPARE_CELLS_H

#include "Cell.h"

// util class to Astar

class CompareCells
{
public:
	CompareCells() = delete;
	bool operator()(const Cell& c1, const Cell& c2) const { return c1.getF() > c2.getF(); }	
};

#endif // __COMPARE_CELLS_H