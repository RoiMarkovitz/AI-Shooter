#ifndef __ROOM_H
#define __ROOM_H

#include "Point2D.h"

class Point2D;

class Room
{
private:
	int width, height;
	Point2D centerPoint;
	int roomId;
	
public:
	Room() = delete;
	Room(int cr, int cc, int w, int h, int id);

	int getW() const { return width; }
	int getH() const { return height; }
	int getRoomId() const { return roomId; }
	const Point2D& getCenterPoint() const { return centerPoint; }
};

#endif // __ROOM_H