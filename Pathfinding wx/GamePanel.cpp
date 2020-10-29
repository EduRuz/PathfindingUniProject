#include "GamePanel.h"
#include "Actor.h"
#include "Player.h"
#include <wx/dcbuffer.h>
#include <iostream>
#include <chrono>
#include <iostream>
#include <fstream>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using the_clock = std::chrono::steady_clock;

/**
Constructer
Subclass of wxPanel. Initializes the variables needed for the class
@param wxFrame: the frame the panel is attached to
*/
GamePanel::GamePanel(wxFrame* parent):wxPanel(parent)
{	
	wxInitAllImageHandlers();
	wxWindow::SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetDoubleBuffered(true);

	player.setX(0);
	player.setY(0);
	player.LoadFile("Images\\Hero.png", wxBITMAP_TYPE_PNG);

	enemy.setX(975);
	enemy.setY(975);
	enemy.LoadFile("Images\\Boss.png", wxBITMAP_TYPE_PNG);

	wall.LoadFile("Images\\GreyWall.png", wxBITMAP_TYPE_PNG);

	testImage1.LoadFile("Images\\Red.png", wxBITMAP_TYPE_PNG);
	
	aStart = new Location;
	aStart->cd.x = enemy.getX() / CELL_SIZE;
	aStart->cd.y = enemy.getY() / CELL_SIZE;

	aEnd = new Location;
	aEnd->cd.x = player.getX() / CELL_SIZE;
	aEnd->cd.y = player.getY() / CELL_SIZE;

	LeeOrAStar = false;

	
	for (int i = 0; i < X_SIZE; i++)
	{
		for (int j = 0; j < Y_SIZE; j++)
		{
			aGrid[i][j] = new Location;
		}
	}
	
	obstacles();
	fillGrid();

	if (LeeOrAStar == true) {
		for (int i = 0; i < 101; i++) {
			the_clock::time_point startTime = the_clock::now();

			for (int i = 0; i < 1000; i++)
			{
				aStar(aStart, aEnd);
				fillGrid();
				enemy.setX(475);
				enemy.setY(475);
				aStart->cd.x = enemy.getX() / CELL_SIZE;
				aStart->cd.y = enemy.getY() / CELL_SIZE;
			}

			the_clock::time_point endTime = the_clock::now();
			int64_t time_taken = duration_cast<milliseconds>(endTime - startTime).count();
			std::ofstream myfile;
			if (enemy.getY() == 475) {
				myfile.open("AStar20.txt", std::fstream::app);
			}
			else if (enemy.getY() == 975) {
				myfile.open("AStar40.txt", std::fstream::app);
			}

			if (time_taken >= 100 && time_taken < 1000) {
				myfile << 0 << "." << time_taken << ",";
			}
			else {
			myfile << time_taken << ",";
			}
			
			myfile.close();
		}
		//exit(0);
	}
	
	if (LeeOrAStar == false) {
		for (int i = 0; i < 100; i++) {
			the_clock::time_point startTime = the_clock::now();

			for (int i = 0; i < 1000; i++)
			{			
				
				leeAlgP1();
				leeAlgP2();
				fillGrid();
				enemy.setX(975);
				enemy.setY(975);

			}

			the_clock::time_point endTime = the_clock::now();
			int64_t time_taken = duration_cast<milliseconds>(endTime - startTime).count();
			std::ofstream myfile;
			if (enemy.getY() == 475) {
				myfile.open("Lee20.txt", std::fstream::app);
			}
			else if (enemy.getY() == 975) {
				myfile.open("Lee40.txt", std::fstream::app);
			}
			if (time_taken < 100) {
				//myfile << 0 << "." << 0 << time_taken << ",";
			}
			else if (time_taken >= 100) {
				//myfile << 0 << "." << time_taken << ",";
			}

			myfile.close();
		}
		//exit(0);

	}

	playerMoved = false;
	delay = true;
}

/**
Deconstructer.
*/
GamePanel::~GamePanel()
{
	delete aStart;
	delete aEnd;

	for(int i = 0; i < X_SIZE; i++)
	{
		for(int j = 0; j < Y_SIZE; j++)
		{
			if(aGrid[i][j])
			{
				delete aGrid[i][j];
			}
		}
	}
}

/**
Paint event sent when a windows contents needs repainted.
Paints to a buffer before painting to the window.
@param PaintEvent**.
*/
void GamePanel::paintEvent(wxPaintEvent & evt)
{
	wxBufferedPaintDC dc(this);
	render(dc);
}

/**
Called to repaint on to the window.
*/
void GamePanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

/**
Renders the stationary items.
@param wxDC: the device context to be drawn onto.
*/
void GamePanel::renderGrid(wxDC & dc)
{
	dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick

	for(int i = 0; i <= (CELL_SIZE * X_SIZE); i += CELL_SIZE) {
		dc.DrawLine(0, i, (CELL_SIZE * X_SIZE), i);
	}

	for (int i = 0; i <= (CELL_SIZE * Y_SIZE); i += CELL_SIZE) {
		dc.DrawLine(i, 0, i, (CELL_SIZE * Y_SIZE));
	}

	if (LeeOrAStar == false) {
		for (int i = 0; i < X_SIZE;i++) {
			for (int j = 0; j < Y_SIZE; j++) {
				if (grid[i][j] == -2) {
					dc.DrawBitmap(wxBitmap(wall), i*CELL_SIZE, j*CELL_SIZE, true /* use mask */);
				}
			}
		}
	}

	if (LeeOrAStar == true) {
		std::set<GamePanel::Location *>::iterator it;
		for (it = blocked.begin(); it != blocked.end(); ++it)
		{
			dc.DrawBitmap(wxBitmap(wall), (*it)->cd.x * CELL_SIZE, (*it)->cd.y * CELL_SIZE);

		}
	}
}

/**
Main render method.
@param wxDC: the device context to be drawn onto.
*/
void GamePanel::render(wxDC & dc)
{
	wxMilliSleep(50);
	dc.Clear();

	if (playerMoved == true) {
		
		aStart->cd.x = enemy.getX() / CELL_SIZE;
		aStart->cd.y = enemy.getY() / CELL_SIZE;

		aEnd->cd.x = player.getX() / CELL_SIZE;
		aEnd->cd.y = player.getY() / CELL_SIZE;

		fillGrid();

		if (LeeOrAStar == false) {
			leeAlgP1();
			leeAlgP2();
		}

		if (LeeOrAStar == true) {
			aStar(aStart, aEnd);
		}

		playerMoved = false;

	}
	//renderEnemy(dc)

	renderPath(dc);

	renderGrid(dc);
	//dc.DrawBitmap(wxBitmap(player), player.getX(), player.getY(), true /* use mask */);
	
}

/**
Renders the enemy.
@param wxDC: the device context to be drawn onto.
*/
void GamePanel::renderEnemy(wxDC & dc)
{
	//A* Enemy Render
	if (LeeOrAStar == true) {
		if (((vIndx) > 1) && delay == true) {
			Coord curCoord = path2.at(vIndx - 1)->cd;
			enemy.setX(curCoord.x * CELL_SIZE);
			enemy.setY(curCoord.y * CELL_SIZE);
			dc.DrawBitmap(wxBitmap(enemy), enemy.getX(), enemy.getY(), true /* use mask */);
			vIndx--;
		}
		else {
			dc.DrawBitmap(wxBitmap(enemy), enemy.getX(), enemy.getY(), true /* use mask */);
		}
	}

	//Lee Enemy Render
	if (LeeOrAStar == false) {
		if ((vIndx < vectSize - 1) && delay == true) {
			Coord curCoord = sPath.at(vIndx);
			enemy.setX(curCoord.x * CELL_SIZE);
			enemy.setY(curCoord.y * CELL_SIZE);
			dc.DrawBitmap(wxBitmap(enemy), enemy.getX(), enemy.getY(), true /* use mask */);
			vIndx++;
		}
		else {
			dc.DrawBitmap(wxBitmap(enemy), enemy.getX(), enemy.getY(), true /* use mask */);
		}
	}

	if (delay == true) {
		delay = false;
	}
	else {
		delay = true;
	}
}

/**
Renders the path of the algorithim.
@param wxDC: the device context to be drawn onto.
*/
void GamePanel::renderPath(wxDC & dc) {

	std::set<GamePanel::Location *>::iterator it;

	if (LeeOrAStar == true) {
		for (int i = path2.size() - 1; i > 0; i--)
		{
			dc.DrawBitmap(wxBitmap(testImage1), path2.at(i)->cd.x * CELL_SIZE, path2.at(i)->cd.y * CELL_SIZE);
		}
	}

	if (LeeOrAStar == false) {
		for (int i = sPath.size() - 1; i >= 0; i--)
		{
			dc.DrawBitmap(wxBitmap(testImage1), sPath.at(i).x * CELL_SIZE, sPath.at(i).y * CELL_SIZE);
		}
	}
}

/**
Fills the pathfinding grid with the values needed to work out the path,
for every loop, assign athe adjacent values that havent been assigned a value, increasing every loop.
*/
void GamePanel::leeAlgP1()
{
	end.x = player.getX() / CELL_SIZE;
	end.y = player.getY() / CELL_SIZE;

	start.x = enemy.getX() / CELL_SIZE;
	start.y = enemy.getY() / CELL_SIZE;

	grid[start.x][start.y] = 0;
	int tracker = 1;
	int arrayLimit = X_SIZE -1;

	while (grid[end.x][end.y] <= -1) {

		for (int i = 0; i < Y_SIZE;i++) {

			for (int j = 0; j < Y_SIZE; j++) {

				if (grid[i][j] == (tracker - 1)) {
					if (i == arrayLimit && j == arrayLimit) {
						if (grid[i - 1][j] == -1) {
							grid[i - 1][j] = tracker;
						}
						if (grid[i][j - 1] == -1) {
							grid[i][j - 1] = tracker;
						}

					}
					else if (i == arrayLimit && j != arrayLimit) {
						if (grid[i - 1][j] == -1) {
							grid[i - 1][j] = tracker;
						}
						if (grid[i][j - 1] == -1) {
							grid[i][j - 1] = tracker;
						}
						if (grid[i][j + 1] == -1) {
							grid[i][j + 1] = tracker;
						}
					}
					else if (i != arrayLimit && j == arrayLimit) {
						if (grid[i + 1][j] == -1) {
							grid[i + 1][j] = tracker;
						}
						if (grid[i - 1][j] == -1) {
							grid[i - 1][j] = tracker;
						}
						if (grid[i][j - 1] == -1) {
							grid[i][j - 1] = tracker;
						}
					}
					else {
						if (grid[i + 1][j] == -1) {
							grid[i + 1][j] = tracker;
						}
						if (grid[i - 1][j] == -1) {
							grid[i - 1][j] = tracker;
						}
						if (grid[i][j - 1] == -1) {
							grid[i][j - 1] = tracker;
						}
						if (grid[i][j + 1] == -1) {
							grid[i][j + 1] = tracker;
						}
					}

				}
			}

		}
		tracker++;
	}
}

/**
Creates and fills the Vector data structure that holds the path for pathfinding.
*/
void GamePanel::leeAlgP2()
{
	sPath.clear();
	//sPath.push_back(end);
	current.x = end.x;
	current.y = end.y;
	std::cout << "\n" << current.x << " " << current.y;

	while (grid[current.x][current.y] != grid[start.x][start.y]) {

		if (grid[current.x][current.y] - 1 == grid[current.x + 1][current.y]) {
			current.x = current.x + 1;
			sPath.push_back(current);
		}
		else if (grid[current.x][current.y] - 1 == grid[current.x][current.y + 1]) {
			current.y = current.y + 1;
			sPath.push_back(current);
		}
		else if (grid[current.x][current.y] - 1 == grid[current.x - 1][current.y]) {
			current.x = current.x - 1;
			sPath.push_back(current);
		}
		else if (grid[current.x][current.y] - 1 == grid[current.x][current.y - 1]) {
			current.y = current.y - 1;
			sPath.push_back(current);
		}

	}
	vectSize = sPath.size();
	vIndx = vectSize;
}

/**
Fills the pathfinding grid with -1 for space and -2 for obstacle.
*/
void GamePanel::fillGrid()
{
	
	for (int i = 0; i < X_SIZE; i++) {
		for (int j = 0; j < Y_SIZE; j++) {
			if (grid[i][j] != -2) {
				grid[i][j] = -1;
			}
		}

	}
	
	for (int i = 0; i < X_SIZE; i++) {
		for (int j = 0; j < Y_SIZE; j++) {
			aGrid[i][j]->cd.x = i;
			aGrid[i][j]->cd.y = j;
			aGrid[i][j]->g = 0;
			aGrid[i][j]->f = 0;
			aGrid[i][j]->h = 0;

			if (aGrid[i][j]->obstacle != true) {
				aGrid[i][j]->obstacle = false;
			}
		}
	}


}

/**
Places the obstacle in the respective grids for the pathfinding algorithms.
*/
void GamePanel::obstacles()
{
	for (int i = 1; i < 7; i++) {
		aGrid[0][i]->obstacle = true;
		grid[0][i] = -2;
	}

	for (int i = 0; i < 6; i++) {
		aGrid[19][i]->obstacle = true;
		grid[19][i] = -2;
	}

	for (int i = 2; i < 7; i++) {
		aGrid[i][0]->obstacle = true;
		grid[i][0] = -2;
	}

	for (int i = 9; i < 14; i++) {
		aGrid[4][i]->obstacle = true;
		grid[4][i] = -2;
	}

	for (int i = 10; i < 17; i++) {
		aGrid[i][3]->obstacle = true;
		grid[i][3] = -2;
	}

	for (int i = 1; i < 4; i++) {
		aGrid[9][i]->obstacle = true;
		grid[9][i] = -2;
	}

	for (int i = 4; i < 19; i++) {
		aGrid[9][i]->obstacle = true;
		grid[9][i] = -2;
	}

	for (int i = 2; i < 8; i++) {
		for (int j = 2; j < 7; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 11; i < 18; i++) {
		for (int j = 11; j < 18; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 9; i < 14; i++) {
		aGrid[i][20]->obstacle = true;
		grid[i][20] = -2;
	}

	for (int i = 15; i < 20; i++) {
		aGrid[i][20]->obstacle = true;
		grid[i][20] = -2;
	}

	for (int i = 21; i < 24; i++) {
		aGrid[11][i]->obstacle = true;
		grid[11][i] = -2;
	}

	for (int i = 21; i < 24; i++) {
		aGrid[17][i]->obstacle = true;
		grid[17][i] = -2;
	}


	for (int i = 12; i < 22; i++) {
		for (int j = 6; j < 9; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 5; i < 19; i++) {
		aGrid[25][i]->obstacle = true;
		grid[25][i] = -2;
	}

	for (int i = 11; i < 25; i++) {
		aGrid[22][i]->obstacle = true;
		grid[22][i] = -2;
	}

	for (int i = 28; i < 39; i++) {
		aGrid[i][26]->obstacle = true;
		grid[i][26] = -2;
	}

	for (int i = 27; i < 39; i++) {
		aGrid[28][i]->obstacle = true;
		grid[28][i] = -2;
	}

	for (int i = 6; i < 16; i++) {
		for (int j = 25; j < 33; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 2; i < 8; i++) {
		for (int j = 16; j < 22; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 30; i < 38; i++) {
		for (int j = 30; j < 37; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 29; i < 39; i++) {
		for (int j = 14; j < 22; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 18; i < 22; i++) {
		for (int j = 25; j < 29; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 18; i < 22; i++) {
		for (int j = 30; j < 34; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 23; i < 27; i++) {
		for (int j = 25; j < 29; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 23; i < 27; i++) {
		for (int j = 30; j < 34; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 3; i < 26; i++) {
		aGrid[i][37]->obstacle = true;
		grid[i][37] = -2;
	}

	for (int i = 28; i < 31; i++) {
		for (int j = 5; j < 8; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 32; i < 35; i++) {
		for (int j = 5; j < 8; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 28; i < 31; i++) {
		for (int j = 9; j < 12; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 32; i < 35; i++) {
		for (int j = 9; j < 12; j++) {
			aGrid[i][j]->obstacle = true;
			grid[i][j] = -2;
		}
	}

	for (int i = 25; i < 33; i++) {
		aGrid[i][2]->obstacle = true;
		grid[i][2] = -2;
	}

	for (int i = 23; i < 40; i++) {
		aGrid[3][i]->obstacle = true;
		grid[3][i] = -2;
	}

	for (int i = 24; i < 37; i++) {
		aGrid[1][i]->obstacle = true;
		grid[1][i] = -2;
	}
}

/**
A* Path finding algorithm, finds the optimal path using the Manhatten distance to avoid checking pointless nodes.
@param Location: Starting location for the pathfinding.
@param Location: End location for the pathfinding.
*/
void GamePanel::aStar(Location *aStart, Location *aEnd)
{	
	open.clear();
	closed.clear();
	blocked.clear();

	aStarSetup(aStart, aEnd);

	while (open.empty() != true) {
		aCurrent = minElement(open);

		if (aCurrent == aGrid[aEnd->cd.x][aEnd->cd.y]) {
			Location *node = aCurrent;
			path2.clear();
			while (node != aGrid[aStart->cd.x][aStart->cd.y]){
				path2.push_back(node);
				node = node->parent;
				
			}
			vectSize = path2.size();
			vIndx = path2.size();
			break;
		
		}
		open.erase(aCurrent);
		closed.insert(aCurrent);
		int new_g = 0;
		std::set<GamePanel::Location *>::iterator it;
		for (int i = 0; i < aCurrent->neighbors.size(); i++) {

			it = closed.find(aCurrent->neighbors.at(i));
			if (it != closed.end() || aCurrent->neighbors.at(i)->obstacle == true) {
				continue;
			}
			
			new_g = aCurrent->g + dist_between(aCurrent, aCurrent->neighbors.at(i));
			
			it = open.find(aCurrent->neighbors.at(i));
			if (it == open.end()) {
				open.insert(aCurrent->neighbors.at(i));
			}
			else if (new_g >= aCurrent->neighbors.at(i)->g) {
				continue;
			}

			aCurrent->neighbors.at(i)->g = new_g;
			aCurrent->neighbors.at(i)->f = aCurrent->neighbors.at(i)->g + heuristic(aCurrent->neighbors.at(i), aEnd);
			aCurrent->neighbors.at(i)->parent = aCurrent;
		}
	}
}

/**
Sets up the grid neighbors for the A*.
@param Location: Starting location for the pathfinding.
@param Location: End location for the pathfinding.
*/
void GamePanel::aStarSetup(Location * aStart, Location * aEnd)
{
	
	for (int i = 0; i < X_SIZE; i++) {
		for (int j = 0; j < Y_SIZE; j++) {
			aGrid[i][j]->neighbors.clear();
			if (i < X_SIZE - 1) {
				aGrid[i][j]->neighbors.push_back(aGrid[i + 1][j]);
			}
			if (i > 0) {
				aGrid[i][j]->neighbors.push_back(aGrid[i -1][j]);
			}
			if (j < Y_SIZE - 1) {
				aGrid[i][j]->neighbors.push_back(aGrid[i][j + 1]);
			}
			if (j > 0) {
				aGrid[i][j]->neighbors.push_back(aGrid[i][j - 1]);
			}
		}
	}



	for (int i = 0; i < X_SIZE; i++) {
		for (int j = 0; j < Y_SIZE; j++) {
			if (aGrid[i][j]->obstacle == true) {
				blocked.insert(aGrid[i][j]);
			}
		}
	}

	aGrid[aStart->cd.x][aStart->cd.y]->g = 0;
	aGrid[aStart->cd.x][aStart->cd.y]->f = aGrid[aStart->cd.x][aStart->cd.y]->g + heuristic(aStart, aEnd);

	open.insert(aGrid[aStart->cd.x][aStart->cd.y]);

	
}

/**
Returns the distance between the passed nodes. (Always returns 0 because of the NSEW movement of the grid)
@param Location: Current location being evaluated.
@param Location: The Neighbor Node to the current node to be evaluated.
*/
int GamePanel::dist_between(Location *current, Location *node)
{
	return 1;
}

/**
Returns the Heuristic value using the Manhatten distance to calculate the optimal path to try.
@param Location: Current location being evaluated.
@param Location: The Neighbor Node to the current node to be evaluated.
*/
int GamePanel::heuristic(Location *aStart, Location *aEnd)
{

	int h = (aStart->cd.x - aEnd->cd.x) + (aStart->cd.y - aEnd->cd.y);
	return h;
}

/**
Returns the lowest f value node of the set passed.
@param set<Location>: Set to be evaluated.
*/
GamePanel::Location * GamePanel::minElement(std::set<Location *> curr)
{
	std::set<GamePanel::Location *>::iterator it;
	it = curr.begin();
	int tempf = (*curr.begin())->f;
	GamePanel::Location * temp = *it;
	for (it = curr.begin(); it != curr.end(); ++it)
	{
		

		if ((*it)->f < tempf) {
			tempf = (*it)->f;
			temp = *it;
		}
	}


	return temp;
}

/**
Manages key presses.
@param key event.
*/
void GamePanel::OnKeyDown(wxKeyEvent & event)
{
	int keycode = event.GetKeyCode();
	
	//e=69 s=83 d=68 f=70
	switch (keycode) {
	case 83:
		if (grid[(player.getX() - CELL_SIZE) / CELL_SIZE][player.getY() / CELL_SIZE] != -2 && aGrid[(player.getX() - CELL_SIZE) / CELL_SIZE][player.getY() / CELL_SIZE]->obstacle != true) {
			player.adjustX(CELL_SIZE, '-');
			player.newImage("Images\\HeroL.png", wxBITMAP_TYPE_PNG);
			playerMoved = true;
		}
		break;
	case 70:
		if (grid[(player.getX() + CELL_SIZE) / CELL_SIZE][player.getY() / CELL_SIZE] != -2 && aGrid[(player.getX() + CELL_SIZE) / CELL_SIZE][player.getY() / CELL_SIZE]->obstacle != true) {
			player.adjustX(CELL_SIZE,'+');
			player.newImage("Images\\HeroR.png", wxBITMAP_TYPE_PNG);
			playerMoved = true;
		}
		break;
	case 69:
		if (grid[player.getX() / CELL_SIZE][(player.getY() - CELL_SIZE) / CELL_SIZE] != -2 && aGrid[player.getX() / CELL_SIZE][(player.getY() - CELL_SIZE) / CELL_SIZE]->obstacle != true) {
			player.adjustY(CELL_SIZE, '-');
			player.newImage("Images\\HeroU.png", wxBITMAP_TYPE_PNG);
			playerMoved = true;
		}
		break;
	case 68:
		if (grid[player.getX() / CELL_SIZE][(player.getY() + CELL_SIZE) / CELL_SIZE] != -2 && aGrid[player.getX() / CELL_SIZE][(player.getY() + CELL_SIZE) / CELL_SIZE]->obstacle != true) {
			player.adjustY(CELL_SIZE, '+');
			player.newImage("Images\\Hero.png", wxBITMAP_TYPE_PNG);
			playerMoved = true;
		}

		break;
	}

	if (keycode == VK_ESCAPE) {
		exit(0);
	}
	
}

BEGIN_EVENT_TABLE(GamePanel, wxPanel)
	EVT_KEY_DOWN(GamePanel::OnKeyDown)
	EVT_PAINT(GamePanel::paintEvent)
END_EVENT_TABLE()