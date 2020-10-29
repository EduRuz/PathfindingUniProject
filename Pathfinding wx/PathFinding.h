#pragma once
#include "GamePanel.h"
#include "Actor.h"

class PathFinding :
	public wxApp
{
public:
	//PathFinding();
	//virtual ~PathFinding();
	virtual bool OnInit() override;

	bool render_loop_on;
	void onIdle(wxIdleEvent& evt);
	void activateRenderLoop(bool on);
	void openConsoleWindow();

	wxFrame *frame;
	GamePanel *gPane;

	static const int X_SIZE = 40;
	static const int Y_SIZE = 40;
	const int CELL_SIZE = 25;
};