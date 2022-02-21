#include "glut.h"
#include <time.h>
#include "Soldier.h"
#include "GameManager.h"
#include "MapCell.h"
#include "Definitions.h"

using namespace std;

GameManager* gameManager;
//bool showSecurityMap = false; 

void initOpenGL(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL)); // initialize random seed
	initOpenGL(argc, argv);
	delete gameManager;
	return 0;
}

void ShowMaze() 
{
	const MapCell*const* map = gameManager->getMap();	
	for(int i = 0; i < MSZ; i++)
		for(int j = 0 ;j < MSZ; j++)
		{
			// pick color
			switch ((int)map[i][j].getMark())
			{
			case (int)MapMarks::OUTER_WALL:
				glColor3d(0, 0, 0); // set color black
				break;
			case (int)MapMarks::OBSTACLE:
				glColor3d(0.35, 0.2, 0.09); // set color dark wood		
				break;
			case (int)MapMarks::CORRIDOR:
				//if (showSecurityMap) // 2 - securityMap[i][j] because default value in securityMap is 1
				//	glColor3d(2 - map[i][j].getRiskLevel(), 2 - map[i][j].getRiskLevel(), 2 - map[i][j].getRiskLevel());
				//else
					glColor3d(0.55, 0.09, 0.09); // set color scarlet 					
				break;
			case (int)MapMarks::HEALTH_STORAGE:
				glColor3d(1, 0.7, 0.7); // set color pinkish
				break;
			case (int)MapMarks::AMMO_STORAGE:
				glColor3d(0.71, 0.65, 0.26); // set color brass
				break;
			case (int)MapMarks::TEAM_ONE_SOLDIER:
				glColor3d(0.13, 0.13, 0.55); // set color navy
				break;
			case (int)MapMarks::TEAM_ONE_SUPPORT:
				glColor3d(0.196078, 0.6, 0.8); // set color sky blue  
				break;
			case (int)MapMarks::TEAM_TWO_SOLDIER:
				glColor3d(0.137, 0.55, 0.137); // set color forest green
				break;
			case (int)MapMarks::TEAM_TWO_SUPPORT:
				glColor3d(0.196, 0.8, 0.196); // set color lime green
				break;
			case (int)MapMarks::SPACE:
				//if(showSecurityMap) // 2 - securityMap[i][j] because default value in securityMap is 1
				//	glColor3d(2 - map[i][j].getRiskLevel(), 2 - map[i][j].getRiskLevel(), 2 - map[i][j].getRiskLevel());
				//else
					glColor3d(0.80, 0.80, 0.80); // set color very light gray					
				break;

			} // switch
			// show the cell of map
			glBegin(GL_POLYGON);
			glVertex2d(j, i); // left-bottom vertex
			glVertex2d(j, (double)i + 1); // left-top vertex
			glVertex2d((double)j + 1, (double)i + 1); // right-top vertex
			glVertex2d((double)j + 1 ,i); // right-bottom vertex
			glEnd();
		}
}

void showSoldierProjectiles(const Soldier* soldier) 
{
	if (soldier)
	{
		if (soldier->getBullet())
			soldier->getBullet()->show();
		if (soldier->getGrenade())
			soldier->getGrenade()->show();
	}
}

void showBulletsAndGrenades(const vector<Player*>& team) 
{
	vector<Player*>::const_iterator itrTeam = team.begin();
	vector<Player*>::const_iterator itrTeamEnd = team.end();

	for (; itrTeam != itrTeamEnd; ++itrTeam)
		showSoldierProjectiles(dynamic_cast<Soldier*>((*itrTeam)));
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowMaze();
	showBulletsAndGrenades(gameManager->getTeam1());
	showBulletsAndGrenades(gameManager->getTeam2());
	
	glutSwapBuffers(); // show all
}

// runs all the time in the background
void idle(int n)
{
	if (!gameManager->getHasGameEnded() && gameManager->getHasGameStarted())
		gameManager->playOneTurn();
	
	glutTimerFunc(MAX_FPS, idle, 0);
	glutPostRedisplay(); // indirect call to display
}

void menu(int choice) 
{
	switch (choice)
	{
		case 0:
			gameManager->startGame();
			break;
		//case 1:
		//	delete gameManager;
		//	gameManager = new GameManager();
		//	break;
		//case 2:
		//	showSecurityMap = !showSecurityMap; // to put in comment later	
		//	break;
	}
}


void initOpenGL(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Shooter");

	glutDisplayFunc(display);
	//glutIdleFunc(idle);
	// add menu
	glutCreateMenu(menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Start Game", 0);
	//glutAddMenuEntry("New Game", 1);
	//glutAddMenuEntry("Toggle RiskMap", 2); 

	glutTimerFunc(MAX_FPS, idle, 0);
	
	glOrtho(0, MSZ, 0, MSZ, -1, 1);
	//     left right bottom top near, far

	gameManager = new GameManager();

	glutMainLoop();
}