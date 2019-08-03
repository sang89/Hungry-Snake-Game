#ifndef SNAKE_H
#define SNAKE_H

#include "Square.h"
#include <vector>
#include "LTexture.h"

class Monster;

class Snake
{
public:
	// Constructor
	Snake();

	// Overloaded constructor
	Snake(int x, int y);

	// Destructor
	~Snake();

	// Draw snake
	void render();
	
	// Move the snake
	bool move(DIRECTION direction);

	// Add square
	void addSquare(DIRECTION direction, int n);

	// Set head
	void setHead(Square* head);

	// Get tail
	Square* getTail() const;

	// Get head
	Square* getHead() const;

	// User controls the snake
	void handleSnake(SDL_Event& e);

	// Get the snake current direction
	DIRECTION getMomentum() const;

	// Check if the snake hits the playing screen
	bool checkHitScreen() const;

	// Check if the snake hits itself
	bool checkHitItself() const;

	// Check if the snake eats food
	bool eatFood(const Square& food) const;

	// Get tail momentum and add square after it (when the snake eats)
	void addAfterEat();

	// Get the snake's length
	int getLength() const { return numSquares; }

	// Get textures
	virtual LTexture getBodyTexture() const;
	virtual LTexture getHeadTexture() const;

	// Check if snake hit a monster
	bool hitMonster(const Monster& monster) const;

protected:
	// Snake momentum
	DIRECTION momentum;

	// Head of the snake
	Square* head;

	// Number of squares
	int numSquares;

	// Body and head texture
	LTexture bodyTexture;
	LTexture headTexture;
};



// Monster class
class Monster : public Snake
{
public:
	// Constructors
	Monster();
	Monster(int x, int y);

	// Handle monster move
	void handleMonster(const std::vector<Square>& bombs);

	// Set sleep state for monster
	void setSleep(bool sleep);

private:
	// Sleep state
	bool sleep;
};


#endif