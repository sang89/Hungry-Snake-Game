#ifndef SBUTTON_H
#define SBUTTON_H
#include "LTexture.h"
#include "constants.h"

class SButton
{
public:
	// Default constructor
	SButton();

	// Overloaded constructor
	SButton(const LTexture& buttonClicked, const LTexture& buttonRest, bool isActive, int w, int h);

	// Render button 
	void render(int x, int y);

	// Get button state
	bool getState() const { return isActive; }

	// Set state
	void setState(bool state);

	// Set texture
	void setTexture(const LTexture& buttonClicked, const LTexture& buttonRest);

	// Set location
	void setLocation(int x, int y);

	// Get location
	int getX() const { return x; }
	int getY() const { return y; }

	// Set size
	void setSize(int w, int h);

	// Get size
	int getWidth() const { return w; }
	int getHeight() const { return h; }

	// Check if button is clicked
	bool buttonIsPressed(SDL_Event& e) const;

	// Check if mouse is over the button
	bool mouseOnButton(SDL_Event& e) const;

	// Translate button
	void translateButton(DIRECTION d, int n);

private:
	// Two textures for button
	LTexture buttonClicked;
	LTexture buttonRest;

	// State of button
	bool isActive;

	// Positions
	int x, y;

	// Size
	int w, h;
};




#endif
