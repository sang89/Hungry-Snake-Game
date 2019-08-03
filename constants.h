/////////////////////////////////////////////
/// This file defines all the constants /////
////////////////////////////////////////////

// Screen constants
#define SCREEN_WIDTH 840
#define SCREEN_HEIGHT 480

// Playing screen
#define PLAYING_BOX_WIDTH 560
#define PLAYING_BOX_HEIGHT 480
#define PLAYING_BOX_X 140
#define PLAYING_BOX_Y 0

// Square size
#define SQUARE_SIZE 20

// Frame rate
#define FRAME_RATE 200

// Move direction for the snake
enum DIRECTION
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	TOTAL
};

// Easy, normal, hard flag
enum LEVEL_FLAG
{
	EASY,
	NORMAL,
	HARD
};

extern LEVEL_FLAG GAME_LEVEL;

// Types of demand when user presses exit buttons
enum DEMAND_TYPE
{
	EXIT,
	GO_BACK,
	CHANGE_LEVEL,
	START_OVER
};

// Button width and height
#define BUTTON_SIZE 28

// Option button when game start width and height
#define OPTION_BUTTON_WIDTH 280
#define OPTION_BUTTON_HEIGHT 80