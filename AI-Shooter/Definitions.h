#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

enum class MapMarks{OUTER_WALL, OBSTACLE, SPACE, CORRIDOR, HEALTH_STORAGE, AMMO_STORAGE,
	 TEAM_ONE_SOLDIER, TEAM_ONE_SUPPORT, TEAM_TWO_SOLDIER, TEAM_TWO_SUPPORT
};
enum class eTeam { null, Team1, Team2 };

enum class eAstar {BuildCorridors, ShortestPath};

const int MSZ = 100;

const int NUM_ROOMS = 10;
const int MAX_ROOM_WIDTH = (MSZ / 5);
const int MAX_ROOM_HEIGHT = (MSZ / 5);
const int MIN_ROOM_WIDTH = 12;
const int MIN_ROOM_HEIGHT = 12;
const int CORRIDOR_ID = -2;

const int OBTACLES_PERCENTAGE_IN_ROOM = 5; 
const int NUM_HEALTH_KITS_IN_ROOM = 2;
const int NUM_AMMO_KITS_IN_ROOM = 2;

const int TEAM_SIZE = 3;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int MAX_FPS = 50; // lower fps for faster game, higher fps for slower game

const double PI = 3.141592653589793238463;

#endif // __DEFINITIONS_H