#pragma once

#include "wx/wx.h"
#include "Actor.h"
#include "Player.h"
#include "Enemy.h"
#include <list>
#include <vector>
#include <set>

class GamePanel : public wxPanel
{

public:
	GamePanel(wxFrame* parent);
	~GamePanel();
	void paintNow();

private:
	struct Coord {
		wxCoord x;
		wxCoord y;
	};

	struct Location {
		Coord cd;
		int f;
		int g;
		int h;
		bool obstacle;

		Location *parent;
		std::vector<Location *> neighbors;
	};
	enum direction {Up, Down, Left, Right};
	
	void paintEvent(wxPaintEvent & evt);
	void renderGrid(wxDC& dc);
	void render(wxDC& dc);
	void renderEnemy(wxDC& dc);
	void renderPath(wxDC & dc);

	void OnKeyDown(wxKeyEvent& event);

	static const int X_SIZE = 40;
	static const int Y_SIZE = 40;
	const int CELL_SIZE = 25;

	bool playerMoved;

	int grid [X_SIZE][Y_SIZE];

	Player player;
	Enemy enemy;
	Actor wall;

	Actor testImage1;

	bool LeeOrAStar;
	int vectSize;
	int vIndx;
	bool delay;

	std::vector<Coord> sPath;
	Coord start;
	Coord end;
	Coord current;

	void leeAlgP1();
	void leeAlgP2();
	void fillGrid();
	void obstacles();

	int dist_between(Location *current, Location *node);
	int heuristic(Location *aStart, Location *aEnd);
	void aStar(Location *aStart, Location *aEnd);
	void aStarSetup(Location *aStart, Location *aEnd);

	std::set<Location *> open;
	std::set<Location *> closed;
	std::vector<Location *> path2;
	std::set<Location *> blocked;

	Location * aGrid[X_SIZE][Y_SIZE];

	Location *aStart;
	Location *aEnd;
	Location *aCurrent;

	Location * minElement(std::set<Location *> curr);

	DECLARE_EVENT_TABLE()

};


