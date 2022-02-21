#ifndef __ASTAR_H
#define __ASTAR_H

#include "CompareCells.h"
#include "Cell.h"
#include "MapCell.h"
#include "Definitions.h"
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

// util class

class MapCell;

class Astar
{
private:
	static void checkNeighbor(Cell* pCurrent, int nrow, int ncol, priority_queue <Cell, vector<Cell>, CompareCells>& pq, vector <Cell>& grays, vector <Cell>& blacks, const MapCell*const* map, eAstar option, const Point2D& targetPoint);
public:
	Astar() = delete;
	static void freePathMemory(Cell* path);
	static Cell* aStar(const Point2D& startPoint, const Point2D& targetPoint, const MapCell*const* map, eAstar option);
};

#endif // __ASTAR_H