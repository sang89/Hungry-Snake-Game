#include "Snake.h"

// Default constructor
Snake::Snake()
{
	// Initialize textures
	bodyTexture = gSquareTexture;
	headTexture = gHeadTexture;

	// Set head
	head = new Square(400, 200);
	head->setNextSquare(NULL);
	head->setTexture(&headTexture);

	// Length of the snake is 1
	numSquares = 1;

	// Initialize momentum
	momentum = LEFT;
}

// Overloaded constructor
Snake::Snake(int x, int y)
{
	// Initialize textures
	bodyTexture = gSquareTexture;
	headTexture = gHeadTexture;

	// Set head
	head = new Square(x, y);
	head->setNextSquare(NULL);
	head->setTexture(&headTexture);

	// Length of the snake is 1
	numSquares = 1;

	// Initialize momentum
	momentum = LEFT;
}

// Destructor
Snake::~Snake()
{
	numSquares = 0;
	delete head;
	head = NULL;
}

// Draw snake
void Snake::render()
{
	Square* current = head;
	while (current->getNextSquare() != NULL)
	{
		current->render();
		current = current->getNextSquare();
	}

	current->render();
}

// Set head
void Snake::setHead(Square* head)
{
	this->head = head;
}

// Get tail
// @return - the pointer to the last square
Square* Snake::getTail() const
{
	if (head == NULL || head->getNextSquare() == NULL)
		return NULL;

	else
	{
		Square* tail = head;
		while (tail->getNextSquare() != NULL)
		{
			tail = tail->getNextSquare();
		}

		return tail;
	}
}

// Get textures
LTexture Snake::getHeadTexture() const
{
	return headTexture;
}

LTexture Snake::getBodyTexture() const
{
	return bodyTexture;
}


// Get head
// @return - the pointer to snake's head
Square* Snake::getHead() const
{
	return head;
}

// Add square to the snake
// @param direction - the direction to add square (this becomes new tail)
// @param n - number of squares added
void Snake::addSquare(DIRECTION direction, int n)
{
	// Initialize translation differences
	int xDiff, yDiff;
	switch (direction)
	{
	case LEFT:
		xDiff = - SQUARE_SIZE;
		yDiff = 0;
		break;
	case RIGHT:
		xDiff = SQUARE_SIZE;
		yDiff = 0;
		break;
	case UP:
		xDiff = 0;
		yDiff = - SQUARE_SIZE;
		break;
	case DOWN:
		xDiff = 0;
		yDiff = SQUARE_SIZE;
		break;
	}


	for (int i = 0; i < n; i++)
	{
		// If the snake has only 1 head (length == 1)
		if (!getTail())
		{
			Square* tail = new Square(head->getX() + xDiff, head->getY() + yDiff);
			head->setNextSquare(tail);
			tail->setNextSquare(NULL);
			tail->setTexture(&getBodyTexture());
		}

		else
		{
			// Add new square following the old tail
			Square* newTail = new Square(getTail()->getX() + xDiff, getTail()->getY() + yDiff);
			getTail()->setNextSquare(newTail);
			newTail->setNextSquare(NULL);
			newTail->setTexture(&getBodyTexture());
		}

		// Increase length of the snake
		numSquares++;
	}
}

// Move the snake
// @param direction - the direction the snake will move
// @return false if direction is the same as its body; return true otherwise
bool Snake::move(DIRECTION direction)
{
	// Illegal right move
	if (momentum == LEFT && direction == RIGHT)
		return false;

	// Illegal left move
	if (momentum == RIGHT && direction == LEFT)
		return false;

	// Illegal down move
	if (momentum == UP && direction == DOWN)
		return false;

	// Illegal up move
	if (momentum == DOWN && direction == UP)
		return false;

	// Initialize x, y difference and x, y velocity
	int xDiff, yDiff;
	switch (direction)
	{
	case LEFT:
		xDiff = - SQUARE_SIZE;
		yDiff = 0;
		momentum = LEFT;
		break;
	case RIGHT:
		xDiff = SQUARE_SIZE;
		yDiff = 0;
		momentum = RIGHT;
		break;
	case UP:
		xDiff = 0;
		yDiff = -SQUARE_SIZE;
		momentum = UP;
		break;
	case DOWN:
		xDiff = 0;
		yDiff = SQUARE_SIZE;
		momentum = DOWN;
		break;
	}

	// Allocate new head
	Square* nextHead = new Square(head->getX() + xDiff, head->getY() + yDiff);
	nextHead->setNextSquare(head);
	head->setTexture(&getBodyTexture());
	head = nextHead;
	head->setTexture(&getHeadTexture());

	// Delete old tail -> allocate next tail
	Square* trail = head;
	while (trail->getNextSquare() != getTail())
	{
		trail = trail->getNextSquare();
	}

	Square* tail = trail->getNextSquare();
	delete tail;
	tail = NULL;
	trail->setNextSquare(NULL);

	return true;
}

// Handle input key press for the snake move
void Snake::handleSnake(SDL_Event& e)
{
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:
			if (momentum != RIGHT)
			{
				momentum = LEFT;
			}
			break;
		case SDLK_RIGHT:
			if (momentum != LEFT)
			{
				momentum = RIGHT;
			}
			break;
		case SDLK_UP:
			if (momentum != DOWN)
			{
				momentum = UP;
			}
			break;
		case SDLK_DOWN:
			if (momentum != UP)
			{
				momentum = DOWN;
			}
			break;
		}

}

// Get the snake direction
DIRECTION Snake::getMomentum() const
{
	return momentum;
}


// Check if the snake hit screen
bool Snake::checkHitScreen() const
{
	int x = head->getX();
	int y = head->getY();
	
	// Check hit vertical playing screen
	if (x < PLAYING_BOX_X || x > PLAYING_BOX_X + PLAYING_BOX_WIDTH - SQUARE_SIZE)
		return true;

	// Check hit horizontal playing screen
	else if (y < PLAYING_BOX_Y || y > PLAYING_BOX_Y + PLAYING_BOX_HEIGHT - SQUARE_SIZE)
		return true;

	return false;
}

// Check if the snakes hits itself
bool Snake::checkHitItself() const
{
	Square* trail = head;
	while (trail->getNextSquare() != NULL)
	{
		trail = trail->getNextSquare();
		if (trail->getX() == head->getX() && trail->getY() == head->getY())
			return true;
	}

	return false;
}

// Check if the snake eats food
bool Snake::eatFood(const Square& food) const
{
	if (head->getX() == food.getX() && head->getY() == food.getY())
		return true;

	return false;
}


// addAfterEat - add a new tail after the snake eats
void Snake::addAfterEat()
{
	// Tail momentum
	DIRECTION momentum;

	// First, get the tail momentum
	Square* trail = head; // Trail eventually points to the square next to tail
	
	while (trail->getNextSquare() != getTail())
	{
		trail = trail->getNextSquare();
	}

	if (trail->getX() > getTail()->getX())
		momentum = RIGHT;

	else if (trail->getX() < getTail()->getX())
		momentum = LEFT;

	else if (trail->getY() < getTail()->getY())
		momentum = DOWN;

	else momentum = UP;

	// Add square opposite direction to the tail's momentum
	switch (momentum)
	{
	case LEFT:
		addSquare(RIGHT, 1);
		break;
	case RIGHT:
		addSquare(LEFT, 1);
		break;
	case UP:
		addSquare(DOWN, 1);
		break;
	case DOWN:
		addSquare(UP, 1);
		break;
	}
}

// Check if snake hit monster
// @return - true if snake's head hits the monster; otherwise return false
bool Snake::hitMonster(const Monster& monster) const
{
	Square* current = monster.getHead();
	while (current != NULL)
	{
		if (head->getX() == current->getX() && head->getY() == current->getY())
			return true;
		else
			current = current->getNextSquare();
	}

	return false;
}

//////////////////////////////////////////////////////////////
///////////  MONSTER /////////////////////////////////////////
//////////////////////////////////////////////////////////////
Monster::Monster()
{
	// Initialize textures
	bodyTexture = gMonsterBodyTexture;
	headTexture = gMonsterHeadTexture;
	
	// Set head
	head = new Square(400, 200);
	head->setNextSquare(NULL);
	head->setTexture(&headTexture);

	// Length of the snake is 1
	numSquares = 1;

	// Initialize momentum
	// Initialize momentum
	switch (rand() % 4)
	{
	case 0:
		momentum = LEFT;
		break;
	case 1:
		momentum = RIGHT;
		break;
	case 2:
		momentum = UP;
		break;
	case 3:
		momentum = DOWN;
		break;
	}

	// Initialize sleep state
	sleep = false;
}


Monster::Monster(int x, int y)
{
	// Initialize textures
	bodyTexture = gMonsterBodyTexture;
	headTexture = gMonsterHeadTexture;

	// Set head
	head = new Square(x, y);
	head->setNextSquare(NULL);
	head->setTexture(&headTexture);

	// Length of the monster is 1
	numSquares = 1;

	// Initialize momentum
	switch (rand() % 4)
	{
	case 0: 
		momentum = LEFT;
		break;
	case 1:
		momentum = RIGHT;
		break;
	case 2: 
		momentum = UP;
		break;
	case 3:
		momentum = DOWN;
		break;
	}

	// Initialize sleep state
	sleep = false;
}


// Handle monster movement
// @param bombs - bombs
void Monster::handleMonster(const std::vector<Square>& bombs)
{
	if (sleep)
		return;

	else
	{
		// Create flags for legal moves
		bool left = true;
		bool right = true;
		bool up = true;
		bool down = true;

		int x = head->getX();
		int y = head->getY();


		// Check if the monster on screen border
		if (x == PLAYING_BOX_X)
			left = false;
		else if (x == PLAYING_BOX_X + PLAYING_BOX_WIDTH - SQUARE_SIZE)
			right = false;

		
		if (y == PLAYING_BOX_Y)
			up = false;
		else if (y == PLAYING_BOX_HEIGHT - SQUARE_SIZE)
			down = false;

		// Check if snake hits bombs
		int size = bombs.size();
		for (int i = 0; i < size; i++)
		{
			if (x == bombs[i].getX() + SQUARE_SIZE && y == bombs[i].getY())
				left = false;
			else if (x == bombs[i].getX() - SQUARE_SIZE && y == bombs[i].getY())
				right = false;
			else if (x == bombs[i].getX() && y == bombs[i].getY() + SQUARE_SIZE)
				up = false;
			else if (x == bombs[i].getX() && y == bombs[i].getY() - SQUARE_SIZE)
				down = false;
		}

		// Moves againt momentum is illegal
		if (momentum == LEFT)
			right = false;
		else if (momentum == RIGHT)
			left = false;
		else if (momentum == UP)
			down = false;
		else if (momentum == DOWN)
			up = false;

		// Flag to determine if monster continue its momentum
		bool keepMomentum = false;

		// There are 75% monster will continues its momentum
		if (left && momentum == LEFT)
		{
			if (rand() % 100 < 75)
			{
				move(LEFT);
				keepMomentum = true;
			}
		}
		else if (right && momentum == RIGHT)
		{
			if (rand() % 100 < 75)
			{
				move(RIGHT);
				keepMomentum = true;
			}
		}
		else if (up && momentum == UP)
		{
			if (rand() % 100 < 75)
			{
				move(UP);
				keepMomentum = true;
			}
		}
		else if (down && momentum == DOWN)
		{
			if (rand() % 100 < 75)
			{
				move(DOWN);
				keepMomentum = true;
			}
		}

		//  Handle monster if it does not keep momentum
		if (!keepMomentum)
		{
			switch (rand() % 2)
			{
			case 0:
				if (left || right)
				{
					switch (rand() % 2)
					{
					case 0:
						if (left) move(LEFT);
						else move(RIGHT);
						break;
					case 1:
						if (right) move(RIGHT);
						else move(LEFT);
						break;
					}
				}

				else if (up || down)
				{
					switch (rand() % 2)
					{
					case 0:
						if (up) move(UP);
						else move(DOWN);
						break;
					case 1:
						if (down) move(DOWN);
						else move(UP);
						break;
					}
				}
				break;

			case 1:
				if (up || down)
				{
					switch (rand() % 2)
					{
					case 0:
						if (up) move(UP);
						else move(DOWN);
						break;
					case 1:
						if (down) move(DOWN);
						else move(UP);
						break;
					}
				}

				else if (left || right)
				{
					switch (rand() % 2)
					{
					case 0:
						if (left) move(LEFT);
						else move(RIGHT);
						break;
					case 1:
						if (right) move(RIGHT);
						else move(LEFT);
						break;
					}
				}
				break;
			}
		}

		// If monster has no way to go, change its momentum and force it goes back
		if (!left && !right && !up && !down)
		{
			switch (momentum)
			{
			case LEFT:
				momentum = RIGHT;
				move(RIGHT);
				break;
			case RIGHT:
				momentum = LEFT;
				move(LEFT);
				break;
			case UP:
				momentum = DOWN;
				move(DOWN);
				break;
			case DOWN:
				momentum = UP;
				move(UP);
				break;
			}
		}
	}
}


// Set sleep state for the monster
// Set true to make the monster sleep; otherwise set false
void Monster::setSleep(bool sleep)
{
	this->sleep = sleep;
}