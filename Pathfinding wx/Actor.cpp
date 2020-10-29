#include "Actor.h"

/**
Subclass of wxImage to hold methods needed for both subclasses Player and Enemy.
*/
Actor::Actor():wxImage()
{
	X = 0;
	Y = 0;
}

/**
Returns Y coordinate.
*/
wxCoord Actor::getX()
{
	return X;
}

/**
Returns X coordinate.
*/
wxCoord Actor::getY()
{
	return Y;
}

/**
Sets X coordinates.
@param wxCoord: value to set the X coordinate to.
*/
void Actor::setX(wxCoord x)
{
	X = x;
}

/**
Sets Y coordinates.
@param wxCoord: value to set the Y coordinate to.
*/
void Actor::setY(wxCoord y)
{
	Y = y;
}

/**
Increment or decrement X coordinate.
@param wxCoord: value to be incremented/decremented.
@param char: choose whether to increment or decrement by using '+' or '-' respectively.
*/
void Actor::adjustX(wxCoord x, char op) {

	switch (op) {
	case '-':
		X -= x;
		break;
	case '+':
		X += x;
		break;
	}
}

/**
Increment or decrement Y coordinate.
@param wxCoord: value to be incremented/decremented.
@param char: choose whether to increment or decrement by using '+' or '-' respectively.
*/
void Actor::adjustY(wxCoord y, char op) {

	switch (op) {
	case '-':
		Y -= y;
		break;
	case '+':
		Y += y;
		break;
	}
}

/**
Changes the image assigned to the object.
@param wxString: path for the new image.
@param wxBitmapType: Type of image being passed.
@param int: Index**.
*/
void Actor::newImage(const wxString & name, wxBitmapType type, int index)
{
	Clear();
	LoadFile(name, type, index);
}
