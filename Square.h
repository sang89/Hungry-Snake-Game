#ifndef SQUARE_H
#define SQUARE_H

#include "global.h"
#include "SButton.h"

class Square
{
public:
	// Constructor
	Square();

	// Overloaded constructor
	Square(int x, int y);

	// Destructor 
	~Square();

	// Render square to screen
	void render();

	// Get next square
	Square* getNextSquare() const { return nextSquare; }

	// Set next square
	void setNextSquare(Square* link);

	// Set texture
	void setTexture(LTexture* texture);

	// Get positions
	int getX() const { return x; }
	int getY() const { return y; }

	// Set position
	void setPositions(int x, int y);

private:
	// Position
	int x;
	int y;

	// Square texture
	LTexture squareTexture;

	// Pointer to next square
	Square* nextSquare;
};


#endif