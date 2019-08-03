#include "Square.h"

// Default constructor
Square::Square()
{
	x = 20;
	y = 20;
}

// Overloaded constructor
// Set the square at a given point
// @param int - the x-coordinate of the square
// @param int - the y-coordinate of the square
Square::Square(int x, int y)
{
	this->x = x;
	this->y = y;
}

// Destructor
Square::~Square()
{
	delete nextSquare;
	nextSquare = NULL;
}

// Set square positions
void Square::setPositions(int x, int y)
{
	this->x = x;
	this->y = y;
}

// Render square to screen
void Square::render()
{
	squareTexture.render(x, y);
}


// Set next square
void Square::setNextSquare(Square* link)
{
	nextSquare = link;
}


// Set texture
void Square::setTexture(LTexture* texture)
{
	squareTexture.free();
	squareTexture = *texture;
}