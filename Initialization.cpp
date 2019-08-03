// This is initialization file

#include "global.h"
#include "constants.h"
#include "media.h"

///////////////////////////////////////////////////////////
// Initialization and prototypes 
///////////////////////////////////////////////////////////

// Initialize gWindow
SDL_Window* gWindow = NULL;

// Initialize gRenderer
SDL_Renderer* gRenderer = NULL;

// Initialize gFont
TTF_Font* gFont = NULL;

// Initialize textures
LTexture gSquareTexture;
LTexture gHeadTexture;
LTexture gMonsterHeadTexture;
LTexture gMonsterBodyTexture;
LTexture gFoodTexture;
LTexture gPlayerTexture;
LTexture gPlayerTextTexture;
LTexture gBlankTexture;
LTexture gTheme;
LTexture gRecord;
LTexture gPlay;
LTexture gBombTexture;

// Initialize window icon
SDL_Surface* gWindowIcon = NULL;

// Initialize game level
LEVEL_FLAG GAME_LEVEL = EASY;

// Initialize musics/chunks
Mix_Music* gPlayMusic = NULL;
Mix_Music* gThemeMusic = NULL;
Mix_Chunk* gSnakeEatSound = NULL;
Mix_Chunk* gMonsterLaugh = NULL;
Mix_Chunk* gBombExplode = NULL;
Mix_Chunk* gCollisionSound = NULL;
Mix_Chunk* gButtonSound = NULL;

// Initialize sound flag
bool playingSoundOn = false;
bool setupSoundOn = false;

// Initialize volume
int gVolume = 0;

// Initialize player name
std::string gName = "Player Name";

// Initialize record global
int HIGHSCORE, FIRST, SECOND, THIRD = 0;
std::string LEVEL_HIGHSCORE, LEVEL_FIRST, LEVEL_SECOND, LEVEL_THIRD = "EASY";

//////////////////////////////////////////////////
// Functions implementations
//////////////////////////////////////////////////

bool init()
{
	// Success flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Cannot initialize SDL! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	else
	{
		// Enable Vsync
		if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"))
			printf("Warning: Vsync is not enabled!\n");

		// Enable linear texture filtering
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			printf("Warning: Linear filtering texture is not enabled!\n");

		// Create window
		gWindow = SDL_CreateWindow("HUNGRY SNAKE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (!gWindow)
		{
			printf("Cannot create window! SDL Error: %s\n", SDL_GetError());
			success = false;
		}

		else
		{
			// Create window renderer
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (!gRenderer)
			{
				printf("Cannot create renderer! SDL Error: %s\n", SDL_GetError());
				success = false;
			}

			else
			{
				// Initialize renderer draw color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				// Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("Cannot intialize IMG! IMG Error: %s\n", IMG_GetError());
					success = false;
				}

				else
				{
					// Initialize TTF
					if (TTF_Init() == -1)
					{
						printf("Cannot initialize TTF! TTF Error: %s\n", TTF_GetError());
						success = false;
					}

					// Initialize mixer
					if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
					{
						printf("Cannot initialize SDL_mixer!\n");
						success = false;
					}
				}
			}
		}
	}
	return success;
}


// Load media
bool loadMedia()
{
	// Success flag
	bool success = true;

	// Open font
	gFont = TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 12);
	if (!gFont)
	{
		printf("Cannot open font!\n");
		success = false;
	}

	// Load square texture
	if (!gSquareTexture.loadXPM(body_xpm))
	{
		printf("Cannot load Square Texture!\n");
		success = false;
	}

	// Load head texture
	if (!gHeadTexture.loadXPM(head_xpm))
	{
		printf("Cannot load Head Texture!\n");
		success = false;
	}

	// Load monster head texture
	if (!gMonsterHeadTexture.loadXPM(monster_head_xpm))
	{
		printf("Cannot load Monster Head Texture!\n");
		success = false;
	}

	// Load head texture
	if (!gMonsterBodyTexture.loadXPM(monster_body_xpm))
	{
		printf("Cannot load Monster Body Texture!\n");
		success = false;
	}

	// Load food texture
	if (!gFoodTexture.loadXPM(food_xpm))
	{
		printf("Cannot load Food Texture!\n");
		success = false;
	}

	// Load bomb texture
	if (!gBombTexture.loadXPM(bomb_xpm))
	{
		printf("Cannot load bomb Texture!\n");
		success = false;
	}

	// Load food texture
	if (!gBlankTexture.loadXPM(blank_xpm))
	{
		printf("Cannot load Blank Texture!\n");
		success = false;
	}

	// Load window icon

	gWindowIcon = IMG_ReadXPMFromArray(window_icon_xpm);
	if (!gWindowIcon)
	{
		printf("Cannot load window icon!\n");
		success = false;
	}
	else
	{
		// Set icon for window
		SDL_SetWindowIcon(gWindow, gWindowIcon);
	}
	
	// Load player texture
	if (!gPlayerTexture.loadXPM(player_xpm))
	{
		printf("Cannot load Player Texture!\n");
		success = false;
	}

	// Load player text texture
	SDL_Color textColor = { 0, 0, 0};
	if (!gPlayerTextTexture.loadFromRenderedText(gFont, gName, textColor))
	{
		printf("Cannot load Player Text Texture!\n");
		success = false;
	}

	// Load theme texture
	if (!gTheme.loadXPM(theme_xpm))
	{
		printf("Cannot load Theme Texture!\n");
		success = false;
	}

	// Load theme for record
	if (!gRecord.loadFromFile("resources/record.bmp"))
	{
		printf("Cannot load Record Texture!\n");
		success = false;
	}

	// Load theme for record
	if (!gPlay.loadXPM(play_xpm))
	{
		printf("Cannot load Play Texture!\n");
		success = false;
	}

	// Load snake move chunks
	gPlayMusic = Mix_LoadMUS("resources/music.ogg");
	if (gPlayMusic == NULL)
	{
		printf("Cannot load Snake Move sound!\n");
		success = false;
	}

	// Load them music
	gThemeMusic = Mix_LoadMUS("resources/theme-sound.wav");
	if (gThemeMusic == NULL)
	{
		printf("Cannot load Theme Sound! Error: %s\n", Mix_GetError());
		success = false;
	}

	// Load snake eat sound effect
	gSnakeEatSound = Mix_LoadWAV("resources/snake_eat.wav");
	if (gSnakeEatSound == NULL)
	{
		printf("Cannot load Snake Eat Sound\n");
		success = false;
	}

	// Load monster laugh sound
	gMonsterLaugh = Mix_LoadWAV("resources/monster-laugh.wav");
	if (gMonsterLaugh == NULL)
	{
		printf("Cannot load Monster laugh sound!\n");
		success = false;
	}

	// Load bomb explode sound
	gBombExplode = Mix_LoadWAV("resources/bomb-explode.wav");
	if (gBombExplode == NULL)
	{
		printf("Cannot load Bomb Explode Sound!\n");
		success = false;
	}

	// Load collision sound
	gCollisionSound = Mix_LoadWAV("resources/collision-sound.wav");
	if (gCollisionSound == NULL)
	{
		printf("Cannot load Collision Sound!\n");
		success = false;
	}

	//// Load button sound
	gButtonSound = Mix_LoadWAV("resources/button-sound.wav");
	if (gButtonSound == NULL)
	{
		printf("Cannot load Button Sound! Error: %s\n", Mix_GetError());
		success = false;
	}

	// return success
	return success;
}


// Close SDL and free texture
void close()
{
	// Close font
	TTF_CloseFont(gFont);
	gFont = NULL;

	// Free global texture
	gSquareTexture.free();
	gHeadTexture.free();
	gMonsterHeadTexture.free();
	gMonsterBodyTexture.free();
	gFoodTexture.free();
	gBombTexture.free();
	gPlayerTexture.free();
	gPlayerTextTexture.free();
	gBlankTexture.free();
	gTheme.free();
	gRecord.free();
	gPlay.free();


	// Free sound effects
	Mix_FreeMusic(gPlayMusic);
	gPlayMusic = NULL;
	Mix_FreeMusic(gThemeMusic);
	gThemeMusic = NULL;
	Mix_FreeChunk(gSnakeEatSound);
	gSnakeEatSound = NULL;
	Mix_FreeChunk(gMonsterLaugh);
	gMonsterLaugh = NULL;
	Mix_FreeChunk(gBombExplode);
	gBombExplode = NULL;
	Mix_FreeChunk(gCollisionSound);
	gCollisionSound = NULL;
	Mix_FreeChunk(gButtonSound);
	gButtonSound = NULL;

	// Free surfaces
	SDL_FreeSurface(gWindowIcon);

	// Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	// Quit SDL subsystem
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

