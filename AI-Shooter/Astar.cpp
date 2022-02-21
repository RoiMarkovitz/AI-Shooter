#include "Astar.h"

// runs A*   
Cell* Astar::aStar(const Point2D& startPoint, const Point2D& targetPoint, const MapCell*const* map, eAstar option)
{
	int row, col;
	vector <Cell> grays;
	vector <Cell> blacks;
	priority_queue <Cell, vector<Cell>, CompareCells> pq;
	
	Cell* pstart = new Cell(startPoint.getRow(), startPoint.getCol(), targetPoint.getRow(), targetPoint.getCol(), 0, nullptr);
	Cell* pCurrent;
	// initializes grays and pq
	grays.push_back(*pstart);
	pq.push(*pstart);
	delete pstart; 
	vector<Cell>::iterator it_gray;

	while (!pq.empty())
	{
		pCurrent = new Cell(pq.top());
		pq.pop();
		
		// If current is actually a target then we stop A*	
		if (pCurrent->getPosition().getRow() == targetPoint.getRow() && pCurrent->getPosition().getCol() == targetPoint.getCol()) // then it is target	
		{ //in this case there cannot be a better path to target!!!		
			return pCurrent;
		}
		
		// paint current black
		blacks.push_back(*pCurrent);
		it_gray = find(grays.begin(), grays.end(), *pCurrent); 
		if (it_gray != grays.end()) // current was found
			grays.erase(it_gray); // and removed from grays
		// now check the neighbors of current
		row = pCurrent->getPosition().getRow();
		col = pCurrent->getPosition().getCol();
		// try to go UP (row -1)
		if (row > 0) // we can go UP
			checkNeighbor(pCurrent, row - 1, col, pq, grays, blacks, map, option, targetPoint);

		if (row < MSZ - 1) // DOWN
			checkNeighbor(pCurrent, row + 1, col, pq, grays, blacks, map, option, targetPoint);

		if (col < MSZ - 1) // RIGHT
			checkNeighbor(pCurrent, row, col + 1, pq, grays, blacks, map, option, targetPoint);

		if (col > 0) //LEFT
			checkNeighbor(pCurrent, row, col - 1, pq, grays, blacks, map, option, targetPoint);
	}
	
	return nullptr;
}

// who is the neighbor at nrow ncol? If it is white then paint it gray and add to pq
// If it is gray then check two cases: 
// 1. if F of this neighbor is now better than what was before, we need to update the neighbor
// 2. if it is not better then do nothing
// If it is black do nothing
// If it is Target then we have two cases (actually this is one of the previous cases, white or gray):
// 1. if F of this target is now better than what was before then we need to update the target
// 2. if it is not better then do nothing
void Astar::checkNeighbor(Cell* pCurrent, int nrow, int ncol,
	priority_queue <Cell, vector<Cell>, CompareCells>& pq,
	vector <Cell>& grays, vector <Cell>& blacks, const MapCell*const* map, eAstar option, const Point2D& targetPoint)
{
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	double cost;
	if (option == eAstar::BuildCorridors)
	{
		double space_cost = 0.1, wall_cost = 2;
		if (map[nrow][ncol].getMark() == MapMarks::SPACE || map[nrow][ncol].getMark() == MapMarks::CORRIDOR)
			cost = space_cost;
		else cost = wall_cost;
	}
	else
		cost = map[nrow][ncol].getRiskLevel();
	
	// who can be the neighbor
	if ((nrow == targetPoint.getRow() && ncol == targetPoint.getCol())
		|| map[nrow][ncol].getMark() == MapMarks::SPACE
		|| map[nrow][ncol].getMark() == MapMarks::CORRIDOR 
		|| option == eAstar::BuildCorridors)
	{
		// this is the neighbor
		Cell pn = Cell(nrow, ncol, pCurrent->getTargetPosition().getRow(), pCurrent->getTargetPosition().getCol(),
			pCurrent->getG() + cost, pCurrent);
		
		// check the color of this neighbor
		it_black = find(blacks.begin(), blacks.end(), pn);
		if (it_black != blacks.end()) // it was found i.e. it is black
			return;
		// white
		it_gray = find(grays.begin(), grays.end(), pn);
		if (it_gray == grays.end()) // it wasn't found => it is white
		{
			grays.push_back(pn); // paint it gray
			pq.push(pn);
		}
		else // it is gray
		{
			//   new F       ?      old F
			if (pn.getF() < it_gray->getF()) // then update it (F of neighbor)
			{
				// we need to update it in two places:
				// 1. in vector grays
				it_gray->setG(pn.getG());
				it_gray->setF(pn.getF());
				// 2. in PQ
				// to update a Cell in pq we need to remove it from pq, to update it and to push it back
				vector<Cell> tmp;
				Cell tmpCell;
				do
				{
					if (pq.empty())
					{
						cout << "ERROR! PQ is empty in update PQ\n";
						exit(1);
					}
					tmpCell = pq.top();
					pq.pop();
					if (!(tmpCell == (pn))) // do not copy the Cell we were looking for to tmp! 
						tmp.push_back(tmpCell);
				} while (!(tmpCell == (pn)));
				// now we are out of do-while because we have found the neighbor in PQ. So change it to *pn.
				pq.push(pn);
				// now push back all the elements that are in tmp
				while (!tmp.empty())
				{
					pq.push(tmp.back());
					tmp.pop_back();
				}
			}
		}
	}
}

void Astar::freePathMemory(Cell* path)
{
	Cell* memory;
	if (path)
	{
		while (path->getParent() != nullptr)
		{
			memory = path;
			path = path->getParent();
			delete memory;
		}
		delete path;
	}	
}




