#include "LTexture.h"
#include <stdio.h>
#include "global.h"

// Constructor
LTexture::LTexture()
{
	// Initialize member variables
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}


// Destructor
LTexture::~LTexture()
{
	// Free member texture 
	free();
}


// Free member texture
void LTexture::free()
{
	// Free if the texture different than NULL
	if (mTexture)
	{
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}


// \brief load texture from a specific path
// \param path the string path the texture to be loaded from
// \return true if success, false otherwise
bool LTexture::loadFromFile(std::string path)
{
	// Free preexisting texture
	free();

	// The final texture
	SDL_Texture* newTexture = NULL;

	// Load surface from the specific path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (!loadedSurface)
		printf("Cannot load surface! IMG Error: %s\n", IMG_GetError());

	else
	{
		// Set color key
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0));

		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (!newTexture)
			printf("Cannot create texture from surface pixels! SDL Error: %s\n", SDL_GetError());

		else
		{
			// Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// Get rid of the loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	// Return success
	mTexture = newTexture;

	return mTexture;
}

bool LTexture::loadXPM(char ** name)
{
	// Free preexisting texture
	free();

	// The final texture
	SDL_Texture* newTexture = NULL;

	// Load surface from the specific path
	SDL_Surface* loadedSurface = IMG_ReadXPMFromArray(name);
	if (!loadedSurface)
		printf("Cannot load surface! Cannot open IMG Error: %s\n", IMG_GetError());

	else
	{
		// Set color key
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0));

		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (!newTexture)
			printf("Cannot create texture from surface pixels! SDL Error: %s\n", SDL_GetError());

		else
		{
			// Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// Get rid of the loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	// Return success
	mTexture = newTexture;

	return mTexture;
}
// \brief Load texture from rendered text
// \param textureText the texture text
// \param textColor the color of texture text
bool LTexture::loadFromRenderedText(TTF_Font* font, std::string textureText, SDL_Color& textColor)
{
	// Get rid of preexisting texture
	free();

	// The final texture
	SDL_Texture* newTexture = NULL;

	// Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if (!textSurface)
		printf("Cannot render text surface! TTF Error: %s\n", TTF_GetError());

	else
	{
		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (!newTexture)
			printf("Cannot create texture from text surface! SDL Error: %s\n", SDL_GetError());

		else
		{
			// Get texture dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		// Get rid of loaded text surface
		SDL_FreeSurface(textSurface);
	}

	// Return success
	mTexture = newTexture;
	return mTexture;
}


// \brief Set texture color
// \param red the red color index
// \param green the green color index
// \param blue the blue color index
void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}


// \brief Set alpha index
// \param alpha the alpha index modulation
void LTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}


// \brief Set blending mode
// \param blending the SDL Blending mode
void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
}


// \brief Render the member texture to screen
// \param x the x-coordinate
// \param y the y-coordinate
// \param clip the quad portion of source quad to be rendered
// \param angle the angle of rotation
// \param center the central point of rotation
// \param flip the flip style
void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	// Set rendering space
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	// Set rendering space dimensions
	if (clip)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// Render texture to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}


// Assignment operator overload
LTexture& LTexture::operator ==(const LTexture& texture)
{
	// Check for self assignment
	if (this == &texture) return *this;

	else
	{
		mTexture = texture.mTexture;
		mWidth = texture.mWidth;
		mHeight = texture.mHeight;
	}

	return *this;
}

// Copy constructor
LTexture::LTexture(const LTexture& rhs)
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	*this = rhs;
}