/**
CMP201 Assignment 1
PathFinding.cpp
Purpose: 2D dungeon crawler to demostrate and test the performance of the 
Lee PathFinding Alogorithm against the A* PathFinding Alogorithm.

@author Ruaraidh Canaway
@version 1.0
*/
#include <wx/wx.h>
#include <wx/image.h>
#include "PathFinding.h"
#include "GamePanel.h"
#include "Actor.h"

#ifdef _UNICODE
#ifdef _DEBUG
#pragma comment(lib, "wxbase31ud.lib")
#else
#pragma comment(lib, "wxbase31u.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "wxbase31d.lib")
#else
#pragma comment(lib, "wxbase31.lib")
#endif
#endif

/**
main, run on start up.
*/
bool PathFinding::OnInit()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	frame = new wxFrame(NULL, wxID_ANY, L"MyProject", wxDefaultPosition, wxSize((X_SIZE * CELL_SIZE) + 17, (Y_SIZE * CELL_SIZE) + 40));
	gPane = new GamePanel((wxFrame*)frame);

	openConsoleWindow();

	sizer->Add(gPane, 1, wxEXPAND);

	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);

	frame->Show(true);
	activateRenderLoop(true);
	return true;
}

/**
Runs contents every loop.
@param wxIdleEvent: .**
*/
void PathFinding::onIdle(wxIdleEvent & evt)
{
	if (render_loop_on)
	{
		gPane->paintNow();
		//gPane->Refresh();
		evt.RequestMore(); // render continuously, not only once on idle
	}
}

/**
Activate or deactivate the render loop.
@param boolean: true to activate, false to deactivate.
*/
void PathFinding::activateRenderLoop(bool on)
{
	if (on && !render_loop_on)
	{
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(PathFinding::onIdle));
		render_loop_on = true;
	}
	else if (!on && render_loop_on)
	{
		Disconnect(wxEVT_IDLE, wxIdleEventHandler(PathFinding::onIdle));
		render_loop_on = false;
	}
}

void PathFinding::openConsoleWindow()
{
	// Allocate a console for this app.
	AllocConsole();

	// Make the screen buffer big enough to let us scroll text.
	const WORD MAX_CONSOLE_LINES = 500;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// Redirect the C IO streams (stdout etc.) to the console.
	FILE *old;
	freopen_s(&old, "CONIN$", "r", stdin);
	freopen_s(&old, "CONOUT$", "w", stdout);
	freopen_s(&old, "CONOUT$", "w", stderr);

	// Redirect the C++ IO streams (cout etc.) to the console.
	std::ios::sync_with_stdio();
}

wxIMPLEMENT_APP(PathFinding);

