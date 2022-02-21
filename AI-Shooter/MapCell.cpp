#include "MapCell.h"

MapCell::MapCell()
{
	player = nullptr;
	storage = nullptr;
	roomId = -1;
	riskLevel = MIN_RISK_LEVEL;
}


MapCell::MapCell(int r, int c, MapMarks m)
{
	position = Point2D(r, c);
	mark = m;
	player = nullptr;
	storage = nullptr;
	roomId = -1;
	riskLevel = MIN_RISK_LEVEL;
}


