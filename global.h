#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "LTexture.h"
#include <string>

// The window we will be rendering to
extern SDL_Window* gWindow;

// The window renderer
extern SDL_Renderer* gRenderer;

// The globally used font
extern TTF_Font* gFont;

// The body square texture
extern LTexture gSquareTexture;

// The head texture
extern LTexture gHeadTexture;

// The food texture
extern LTexture gFoodTexture;

// The monster head texture
extern LTexture gMonsterHeadTexture;

// The monster body texture
extern LTexture gMonsterBodyTexture;

// The bomb texture
extern LTexture gBombTexture;

// The window icon surface
extern SDL_Surface* gWindowIcon;

// The player texture
extern LTexture gPlayerTexture;

// The player text
extern LTexture gPlayerTextTexture;

// The blank texture
extern LTexture gBlankTexture;

// The theme texture
extern LTexture gTheme;

// The theme for record
extern LTexture gRecord;

// The theme for play
extern LTexture gPlay;

//The snake sound
extern Mix_Music* gPlayMusic;
extern Mix_Music* gThemeMusic;
extern Mix_Chunk* gSnakeEatSound;
extern Mix_Chunk* gMonsterLaugh;
extern Mix_Chunk* gBombExplode;
extern Mix_Chunk* gCollisionSound;
extern Mix_Chunk* gButtonSound;

// Flag to set playing sound on
extern bool playingSoundOn;

// Flag to set set_up sound on
extern bool setupSoundOn;

// Volume
extern int gVolume;

// Player name
extern std::string gName;

// Record global 
extern int HIGHSCORE;
extern int FIRST;
extern int SECOND;
extern int THIRD;
extern std::string LEVEL_HIGHSCORE;
extern std::string LEVEL_FIRST;
extern std::string LEVEL_SECOND;
extern std::string LEVEL_THIRD;

//// Reference for button ////
// buttons[0] - the play button
// buttons[1] - the pause button
// buttons[2] - the sound on/off button
// buttons[3] - the play-again button
// buttons[4] - the menu button

