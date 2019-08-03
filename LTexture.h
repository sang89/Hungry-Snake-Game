// This class is the texture wrapper

#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

// Texture wrapper class
class LTexture
{
public:
	// Constructor
	LTexture();

	// Copy constructor
	LTexture(const LTexture& rhs);

	// Destructor
	~LTexture();

	// Free member texture
	void free();

	// Load texture at a specific path
	bool loadFromFile(std::string path);

	// Load texture in a xpm file
	bool loadXPM(char** name);

	// Load rendered text
	bool loadFromRenderedText(TTF_Font* gFont, std::string textureText, SDL_Color& textColor);

	// Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	// Set alpha modulation
	void setAlpha(Uint8 alpha);

	// Set blend mode
	void setBlendMode(SDL_BlendMode blending);

	// Render member texture to screen
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL,
		SDL_RendererFlip flip = SDL_FLIP_NONE);

	// Get texture's dimensions
	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

	// Assignment operator overload
	LTexture& operator ==(const LTexture& texture);

	// Get texture
	SDL_Texture* getTexture() const { return mTexture; }

private:
	// The member texture
	SDL_Texture* mTexture;

	// Member texture's dimensions
	int mWidth;
	int mHeight;
};


#endif