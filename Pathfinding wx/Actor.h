#pragma once
#include "wx/wx.h"
class Actor : public wxImage
{

	public:
		Actor();
		
		wxCoord getX();
		wxCoord getY();
		
		void setX(wxCoord x);
		void setY(wxCoord y);

		void adjustX(wxCoord x, char op);
		void adjustY(wxCoord y, char op);
	
		void newImage(const wxString& name, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);

	private:
		wxCoord X;
		wxCoord Y;
		



		
};