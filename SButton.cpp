#include "SButton.h"
#include "global.h"


// Default constructor
SButton::SButton()
{
	buttonClicked.free();
	buttonRest.free();
	isActive = false;
}

// Overloaded constructor
SButton::SButton(const LTexture& buttonClicked, const LTexture& buttonRest, bool isActive, int w, int h)
{
	this->isActive = isActive;
	this->buttonClicked = buttonClicked;
	this->buttonRest = buttonRest;
	this->w = w;
	this->h = h;
}

// Render button
// @param x - the x-coordinate to be rendered
// @param y - the y-coordinate to be rendered
void SButton::render(int x, int y)
{
	if (isActive)
		buttonClicked.render(x, y);

	else
		buttonRest.render(x, y);
}


// setState - setState for button
// @param state - true to set button active, false otherwise;
void SButton::setState(bool state)
{
	isActive = state;
}


// Set textures for the button
void SButton::setTexture(const LTexture& buttonClicked, const LTexture& buttonRest)
{
	this->buttonClicked = buttonClicked;
	this->buttonRest = buttonRest;
}

// Set locations
void SButton::setLocation(int x, int y)
{
	this->x = x;
	this->y = y;
}


// Set size for button
void SButton::setSize(int width, int height)
{
	w = width;
	h = height;
}


// Check if button is pressed
// @return - true if button is pressed
bool SButton::buttonIsPressed(SDL_Event& e) const
{
	if (e.type == SDL_MOUSEBUTTONDOWN && mouseOnButton(e))
		return true;
	return false;
}


// Check if mouse is over the button
bool SButton::mouseOnButton(SDL_Event& e) const
{
	// Get mouse positions
	int x, y;
	SDL_GetMouseState(&x, &y);

	// Check if mouse is in button area
	if (x > this->x && x < this->x + w && y > this->y && y < this->y + h)
			return true;

	return false;
}


// Translate button (while rendering) thru the screen
// @param d - direction that button moves
// @param n - pixels move

void SButton::translateButton(DIRECTION d, int n)
{
	// Pace 
	int pace = 50;
	int xDiff, yDiff;

	for (int i = 0; i <= n / pace; i++)
	{
		int x1 = x;
		int y1 = y;
		int w1 = w;
		int h1 = h;
		switch (d)
		{
		case LEFT:
			x1 = x + w - i * pace;
			y1 = y;
			w1 = i * pace;
			h1 = h;
			xDiff = -1;
			yDiff = 0;
			break;
		case RIGHT:
			x1 = x;
			y1 = y;
			w1 = i * pace;
			h1 = h;
			xDiff = 1;
			yDiff = 0;
			break;
		case UP:
			x1 = x;
			y1 = y + w - i * pace;
			w1 = w;
			h1 = i * pace;
			xDiff = 0;
			yDiff = -1;
			break;
		case DOWN:
			x1 = x;
			y1 = y1;
			w1 = w1;
			h1 = i * pace;
			xDiff = 0;
			yDiff = 1;
			break;
		}

		// Draw background
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_Rect background = { x1, y1, w1, h1 };
		SDL_RenderFillRect(gRenderer, &background);

		// Draw theme
		gTheme.render(0, 0);

		// Draw button at new position
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		render(x + i * xDiff * pace, y + i * yDiff * pace);

		SDL_Delay(150);
		SDL_RenderPresent(gRenderer);
		SDL_RenderClear(gRenderer);
	}

	x = x + xDiff * (n / pace) * pace;
	y = y + yDiff * (n / pace) * pace;
}



