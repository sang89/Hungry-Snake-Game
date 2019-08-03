#include "Scene.h"
#include <cstdlib>
#include <time.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include "media.h"
using namespace std;

// Startup game
void start_up(bool& exit)
{
	// Event handler
	SDL_Event e;

	// Flag to indicate if player wants to jump to next scene
	bool jumpToNextScene = false;

	// Flag to go back
	bool goBack = false;

	do
	{
		// Initiate flag
		goBack = false;

		// Button for this scene
		SButton buttons[4];
		createOptionButton(buttons);

		// Clear renderer
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		// Main loop
		while (!exit && !jumpToNextScene && !goBack)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				// Check if player wants to quit
				if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
					displayExitMessage(exit, EXIT);

				// Handle event on option buttons
				handleEventStartup(e, buttons, jumpToNextScene, exit, goBack);
			}

			// Update screen
			SDL_RenderPresent(gRenderer);
			
		} // end of main loop

		SDL_RenderClear(gRenderer);
	
	} while (goBack);


}// end of start up


// Playing game
// @return - true if player wants to go back to game menu
bool play_game(bool& exit, bool& end)
{
	// Go back to main menu flag
	bool goBack = false;

	// Start-over game flag
	bool startOver = true;
	while (startOver && !goBack && !end)
	{
		// Buttons
		SButton buttons[6]; // See buttons reference at global.h
		createButton(buttons);

		// Snake
		Snake snake;
		snake.addSquare(RIGHT, 4);
		snake.addSquare(DOWN, 3);
		snake.addSquare(LEFT, 3);

		// Create monster
		Monster monster;
		createMonster(monster, snake);

		// Main loop flag
		bool gameOver = false;
		bool pause = true;
		bool quit = false;

		// Event handler
		SDL_Event e;

		// Create food at a randow pos when the game starts
		Square food;
		createFood(food);
		while (foodOnSnake(food, snake) || foodOnSnake(food, monster))
			createFood(food);

		// Create bombs at a random pos not the same as food
		std::vector<Square> bombs;
		createBomb(bombs, food, snake, monster);
	
		// Monster frame rate
		int monsterFrame = 0;


		// Main loop
		while (!quit)
		{

			// Playing music
			if (!quit && Mix_PlayingMusic() == 0 && playingSoundOn)
				Mix_PlayMusic(gPlayMusic, -1);
			// Halt music when player requests so or when game is over
			if (!playingSoundOn)
				Mix_HaltMusic();

			while (SDL_PollEvent(&e) != 0)
			{
				// Handle if player requests quit
				if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
				{
					displayExitMessage(exit, EXIT);

					// If player want to exit
					if (exit)
					{
						quit = true;
						startOver = false;
						break;
					}

					// If player just inadvertent hits exit button, just pause the game
					else
						pause = true;
				}

				// Handle mouse event on buttons
				handleMouseEvent(e, buttons, pause, gameOver, quit, startOver, goBack, end);

				// Handle snake
				if (!gameOver && !pause)
					snake.handleSnake(e);

				// Change play/pause buttons state if game is pause
				buttons[0].setState(!pause); // Change state for the play button
				buttons[1].setState(pause);  // Change state for the pause button

			} // end of poll event


			if (!gameOver && !pause)
			{
				// Clear render
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);
			}


			// Draw Screen
			drawScreen();


			// Render button
			renderButton(buttons, 6);


			if (!gameOver)
			{
				// Render food
				food.render();

				// Render bombs
				int size = bombs.size();
				for (int i = 0; i < size; i++)
					bombs[i].render();

				if (!pause)
				{
					// Snake move
					snake.move(snake.getMomentum());

				}
			}

			// Render and handle monster
			monsterFrame++;
			if (!gameOver && !pause)
			{
				// Handle snake according to its frame rate
				if ( GAME_LEVEL == EASY && monsterFrame > 2)
				{
					monster.handleMonster(bombs);
					monsterFrame = 0;
				}

				else if (GAME_LEVEL == NORMAL && monsterFrame > 4)
				{
					monster.handleMonster(bombs);
					monsterFrame = 0;
				}

				else if (GAME_LEVEL == HARD && monsterFrame > 6)
				{
					monster.handleMonster(bombs);
					monsterFrame = 0;
				}

			}
			monster.render();



			// Render snake
			snake.render();


			if (!gameOver && !pause)
			{
				// Check if the snake or monster eats food
				if (snake.eatFood(food) || monster.eatFood(food))
				{
					if (snake.eatFood(food))
					{
						// Play sound
						if (playingSoundOn)
							Mix_PlayChannel(-1, gSnakeEatSound, 0);

						snake.addAfterEat();
					}

					else if (monster.eatFood(food))
					{
						// Make monster laugh
						if (playingSoundOn)
							Mix_PlayChannel(-1, gMonsterLaugh, 0);

						monster.addAfterEat();
					}

					// Create another food 
					createFood(food);
					// Check if food on snake
					while (foodOnSnake(food, snake) || foodOnSnake(food, monster) || foodOnBomb(food, bombs))
						createFood(food);

				}
			}


			// Render player score
			renderScore(quit, snake);


			// Render time played

		

			// Render game over message
			if (gameOver)
			{
				// Render game over message
				renderGameOver();

				// Pause playing message
				Mix_PauseMusic();
			}

			// Delay according to hard level
			if (GAME_LEVEL == EASY)
				SDL_Delay(150);
			else if (GAME_LEVEL == NORMAL)
				SDL_Delay(100);
			else
				SDL_Delay(70);

			// Check if the snake hits itself / screen / or bomb
			if (!gameOver && !pause)
			{
				if (snake.checkHitItself() || snake.checkHitScreen() || snake.hitMonster(monster))
				{
					// Play sound 
					if (playingSoundOn)
						Mix_PlayChannel(-1, gCollisionSound, 0);

					gameOver = true;
				}

				else
				{
					int size = bombs.size();
					for (int i = 0; i < size; i++)
					if (checkBomb(bombs[i], food, snake))
					{
						// Playing sound 
						if (playingSoundOn)
							Mix_PlayChannel(-1, gBombExplode, 0);

						gameOver = true;
					}
				}

			}

			// Update screen
			SDL_RenderPresent(gRenderer);


		} // end of main loop;

		// Halt the music when game is over
		if (gameOver || goBack)
			Mix_HaltMusic();

		// Output score
		outputScore(snake.getLength() - 11); // Original snake's length is 11
	}

	return goBack;
}

// End game
void end_game()
{
	SDL_Color blue = { 0, 0, 0xff };
	SDL_Color red = { 0xff, 0, 0 };
	SDL_Color black = { 0, 0, 0 };
	
	// Textures
	LTexture greetingTexture1;
	greetingTexture1.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"THANKS FOR PLAYING HUNGRY SNAKE", black);

	LTexture greetingTexture2;
	greetingTexture2.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"*******************************", black);

	LTexture greetingTexture3;
	greetingTexture3.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"CREDITS", black);

	LTexture greetingTexture4;
	greetingTexture4.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"*** Sprites created using GIMP2 and aseprite", black);

	LTexture greetingTexture5;
	greetingTexture5.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"*** Sounds at freesound.org", black);

	LTexture greetingTexture6;
	greetingTexture6.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"*** Sounds edited using Audacity", black);

	LTexture greetingTexture7;
	greetingTexture7.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"*** Coder - Sang Truong ", black);

	LTexture greetingTexture8;
	greetingTexture8.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"Anaheim, California", black);

	LTexture greetingTexture9;
	greetingTexture9.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		"March 26, 2014", black);

	// Frame rate
	int frame = 0;

	// Main loop
	while (frame != 1000)
	{
		// Clear renderer
		SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(gRenderer);

		// Render textures
		greetingTexture1.render(SCREEN_WIDTH * 0.3, SCREEN_HEIGHT  - frame);
		greetingTexture2.render(SCREEN_WIDTH * 0.3, SCREEN_HEIGHT * 1.1 - frame);
		greetingTexture3.render(SCREEN_WIDTH * 0.45, SCREEN_HEIGHT * 1.2 - frame);
		greetingTexture4.render(SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 1.3 - frame);
		greetingTexture5.render(SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 1.4 - frame);
		greetingTexture6.render(SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 1.5 - frame);
		greetingTexture7.render(SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 1.6 - frame);
		greetingTexture2.render(SCREEN_WIDTH * 0.3, SCREEN_HEIGHT * 1.7 - frame);
		greetingTexture8.render(SCREEN_WIDTH * 0.38, SCREEN_HEIGHT * 1.8 - frame);
		greetingTexture9.render(SCREEN_WIDTH * 0.42, SCREEN_HEIGHT * 1.9 - frame);

		// Draw screen
		SDL_RenderPresent(gRenderer);

		// Increment frame
		frame++;
	}

	// Free texture
	greetingTexture1.free();
	greetingTexture2.free();
	greetingTexture3.free();
	greetingTexture4.free();
	greetingTexture5.free();
	greetingTexture6.free();
	greetingTexture7.free();
	greetingTexture8.free();
	greetingTexture9.free();

	// Exit the game
	exit(1);
}

// createMonster - create monster such that it does not lie on snake
void createMonster(Monster& monster, const Snake& snake)
{
	// Set monster's head at a random position
	int x, y;

	bool monsterOnSnake = true;


	do
	{
		int X_MAX = PLAYING_BOX_WIDTH / SQUARE_SIZE - 1;
		int Y_MAX = PLAYING_BOX_HEIGHT / SQUARE_SIZE - 1;

		x = SQUARE_SIZE * (rand() % X_MAX) + PLAYING_BOX_X;
		y = SQUARE_SIZE * (rand() % Y_MAX) + PLAYING_BOX_Y;

		Square* current = snake.getHead();
		while (current != NULL)
		{
			if (current->getX() == x || current->getY() == y)
			{
				monsterOnSnake = true;
				break;
			}

			else
			{
				monsterOnSnake = false;
				current = current->getNextSquare();
			}
		}
		
	} while (monsterOnSnake);
	
	// Set new position for monster's head
	monster.getHead()->setPositions(x, y);

	// Check what direction we should add its body
	if (monster.getHead()->getX() >= PLAYING_BOX_X + 3 * SQUARE_SIZE)
		monster.addSquare(LEFT, 3);
	else
		monster.addSquare(RIGHT, 3);
}

// createFood - create another food if the snake eats the old one
// @param food - the food on the screen
void createFood(Square& food)
{
	srand(time(0));

	int X_MAX = PLAYING_BOX_WIDTH / SQUARE_SIZE - 1;
	int Y_MAX = PLAYING_BOX_HEIGHT / SQUARE_SIZE - 1;

	int x = SQUARE_SIZE * (rand() % X_MAX) + PLAYING_BOX_X;
	int y = SQUARE_SIZE * (rand() % Y_MAX) + PLAYING_BOX_Y;

	food.setPositions(x, y);
	food.setTexture(&gFoodTexture);
	food.setNextSquare(NULL);
} 

// @param bombs - vector of bombs in the scene
void createBomb(std::vector<Square>& bombs, const Square& food, const Snake& snake, const Monster& monster)
{
	int count = 0;
	while (count < 20)
	{
		int X_MAX = PLAYING_BOX_WIDTH / SQUARE_SIZE - 1;
		int Y_MAX = PLAYING_BOX_HEIGHT / SQUARE_SIZE - 1;

		int x = SQUARE_SIZE * (rand() % X_MAX) + PLAYING_BOX_X;
		int y = SQUARE_SIZE * (rand() % Y_MAX) + PLAYING_BOX_Y;

		Square bomb;
		bomb.setPositions(x, y);
		bomb.setNextSquare(NULL);

		if (!checkBomb(bomb, food, snake) && !checkBomb(bomb, food, monster))
		{
			bomb.setTexture(&gBombTexture);
			bombs.push_back(bomb);
			count++;
		}
	}
}

// foodOnSnake - check if generated food lies on snake's body or on bombs
// @param food - the generated food
// @param snake - the snake
// @param boms - vector of bombs
// @return - true if food is on snake's body (including head) or on bombs; return false otherwise
bool foodOnSnake(const Square& food, const Snake& snake)
{
	// Get food positions
	int xFood = food.getX();
	int yFood = food.getY();

	// Get snake head
	Square* head = snake.getHead();
	while (head != NULL)
	{
		int xSnake = head->getX();
		int ySnake = head->getY();
		if (xSnake == xFood && ySnake == yFood)
			return true;

		head = head->getNextSquare();
	}

	return false;
}

// foodOnBomb - check if food is on bombs
// @return - true if food is on bombs; otherwise return false
// @note - return true when food is impossible to get
bool foodOnBomb(const Square& food, const std::vector<Square> bombs)
{
	// Get number of bombs
	int size = bombs.size();

	// Check if food is on bomb
	for (int i = 0; i < size; i++)
	{
		if (food.getX() == bombs[i].getX() && food.getY() == bombs[i].getY())
			return true;
	}

	// Check if food is between 2 bombs and it is impossible to be eaten
	// Number of bombs in food border
	int flag = 0;

	for (int i = 0; i < size; i++)
	{
		if (bombs[i].getX() == food.getX() + SQUARE_SIZE && bombs[i].getY() == food.getY())
				flag++;

		else if (bombs[i].getX() == food.getX() - SQUARE_SIZE && bombs[i].getY() == food.getY())
				flag++;

		else if (bombs[i].getX() == food.getX() && bombs[i].getY() == food.getY() + SQUARE_SIZE)
				flag++;

		else if (bombs[i].getX() == food.getX() && bombs[i].getY() == food.getY() - SQUARE_SIZE)
				flag++;
	}

	// If food is on screen border
	if (food.getX() == PLAYING_BOX_X || food.getX() == PLAYING_BOX_X + PLAYING_BOX_WIDTH - SQUARE_SIZE 
		|| food.getY() == 0 || food.getY() == PLAYING_BOX_HEIGHT - SQUARE_SIZE)
	{
		if (flag == 2)
			return true;
	}

	else
	{
		if (flag == 3)
			return true;
	}

	return false;
}

// checkBomb - check if bomb is on other items
// @return - true if bomb is on other items
bool checkBomb(const Square& bomb, const Square& food, const Snake& snake)
{
	// Get bomb position
	int x = bomb.getX();
	int y = bomb.getY();

	// Check if bomb is on food
	if (x == food.getX() && y == food.getY())
		return true;

	// Check if bomb is on the snake
	Square* head = snake.getHead();
	while (head != NULL)
	{
		if (x == head->getX() && y == head->getY())
			return true;

		head = head->getNextSquare();
	}

	return false;
}

// Render player score
void renderScore(bool& quit, const Snake& snake)
{
	int x = PLAYING_BOX_WIDTH + PLAYING_BOX_X;
	std::stringstream ss;
	ss.str("");
	ss.clear();
	ss << "Score: " << snake.getLength() - 11; // Snake's original length is 11
	SDL_Color scoreColor = { 0xFF, 0, 0 };
	LTexture playerScoreTexture;
	if (!playerScoreTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 22), ss.str(), scoreColor))
	{
		printf("Cannot load score!\n");
		quit = true;
	}
	playerScoreTexture.render(x + (SCREEN_WIDTH - x) / 15, SCREEN_HEIGHT / 2);
	playerScoreTexture.free();
}

// Render GameOver message
void renderGameOver()
{
	SDL_Color gameOverColor = { 0xFF, 0, 0 };
	LTexture gameOverTexture;
	if (!gameOverTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf",80),
		"GAME OVER!", gameOverColor))
		printf("Cannot load game over texture!\n");
	gameOverTexture.render(SCREEN_WIDTH * .23, SCREEN_HEIGHT / 4);
	gameOverTexture.free();

	// Continue message
	SDL_Color gameOverMessageColor = { 0, 0, 0xFF };
	LTexture gameOverMessage;
	if (!gameOverMessage.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 25),
		"Hit Play Button To Continue", gameOverMessageColor))
		printf("Cannot load game over texture!\n");
	gameOverMessage.render(SCREEN_WIDTH * .25, SCREEN_HEIGHT / 2);
	gameOverMessage.free();
}


// Draw screen
void drawScreen()
{
	// Draw playing box
	SDL_Rect quad = { PLAYING_BOX_X, PLAYING_BOX_Y, PLAYING_BOX_WIDTH, PLAYING_BOX_HEIGHT };
	SDL_SetRenderDrawColor(gRenderer, 0, 0xFF, 0x7D, 0xFF);
	SDL_RenderFillRect(gRenderer, &quad);


	// Draw left screen
	SDL_Rect leftQuad = { 0, 0, PLAYING_BOX_X, SCREEN_HEIGHT };
	SDL_SetRenderDrawColor(gRenderer, 0xE3, 0xE3, 0xE3, 0xFF);
	SDL_RenderFillRect(gRenderer, &leftQuad);

	// Draw right screen
	SDL_Rect rightQuad = { PLAYING_BOX_X + PLAYING_BOX_WIDTH, 0,
		SCREEN_WIDTH - PLAYING_BOX_X - PLAYING_BOX_WIDTH, SCREEN_HEIGHT };
	SDL_SetRenderDrawColor(gRenderer, 0xE3, 0xE3, 0xE3, 0xFF);
	SDL_RenderFillRect(gRenderer, &rightQuad);

	// Draw vertical lines for the playing box
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
	SDL_RenderDrawLine(gRenderer, PLAYING_BOX_X, PLAYING_BOX_Y, PLAYING_BOX_X, PLAYING_BOX_Y + PLAYING_BOX_HEIGHT);
	SDL_RenderDrawLine(gRenderer, PLAYING_BOX_X + PLAYING_BOX_WIDTH, PLAYING_BOX_Y,
		PLAYING_BOX_X + PLAYING_BOX_WIDTH, PLAYING_BOX_Y + PLAYING_BOX_HEIGHT);

	// Draw horizontal line for the playing box
	SDL_RenderDrawLine(gRenderer, PLAYING_BOX_X, PLAYING_BOX_Y, PLAYING_BOX_X + PLAYING_BOX_WIDTH, PLAYING_BOX_Y);
	SDL_RenderDrawLine(gRenderer, PLAYING_BOX_X, PLAYING_BOX_Y + PLAYING_BOX_HEIGHT,
		PLAYING_BOX_X + PLAYING_BOX_WIDTH, PLAYING_BOX_Y + PLAYING_BOX_HEIGHT);

	// Draw photo box
	int x = PLAYING_BOX_WIDTH + PLAYING_BOX_X;
	SDL_Rect photoQuad = { x + (SCREEN_WIDTH - x) / 5, SCREEN_HEIGHT / 20,
		3 * (SCREEN_WIDTH - x) / 5, SCREEN_HEIGHT / 4 };
	SDL_RenderDrawRect(gRenderer, &photoQuad);

	gPlayerTexture.render(x + (SCREEN_WIDTH - x) / 5, SCREEN_HEIGHT / 20);
	SDL_RenderDrawRect(gRenderer, &photoQuad);

	//gPlayerTextTexture.render(x + (SCREEN_WIDTH - x) / 5, SCREEN_HEIGHT / 3);
	gPlayerTextTexture.render((PLAYING_BOX_X + PLAYING_BOX_WIDTH + SCREEN_WIDTH - gPlayerTextTexture.getWidth()) / 2, SCREEN_HEIGHT / 3);
}


// Handle Event and draw button
void renderButton(SButton buttons[], int numButtons)
{
	for (int i = 0; i < numButtons; i++)
	{
		buttons[i].render(buttons[i].getX(), buttons[i].getY());
	}
}


// Create initialize button
void createButton(SButton buttons[])
{
	int x = PLAYING_BOX_WIDTH + PLAYING_BOX_X;

	// Set buttons[0] as Play button
	LTexture playRestButtonTexture, playClickedButtonTexture;
	if (!playRestButtonTexture.loadXPM(play_button_rest_xpm)
		|| !playClickedButtonTexture.loadXPM(play_button_clicked_xpm))
		printf("Cannot load play button!\n");

	buttons[0].setTexture(playClickedButtonTexture, playRestButtonTexture);
	buttons[0].setState(false);
	buttons[0].setLocation(x + SCREEN_WIDTH / 45, SCREEN_HEIGHT * 0.9);
	buttons[0].setSize(BUTTON_SIZE, BUTTON_SIZE);
	playRestButtonTexture.free();
	playClickedButtonTexture.free();

	// Set buttons[1] as Pause button
	LTexture pauseRestButtonTexture, pauseClickedButtonTexture;
	if (!pauseRestButtonTexture.loadXPM(pause_button_rest_xpm)
		|| !pauseClickedButtonTexture.loadXPM(pause_button_clicked_xpm))
		printf("Cannot load pause button!\n");

	buttons[1].setTexture(pauseClickedButtonTexture, pauseRestButtonTexture);
	buttons[1].setState(true);
	buttons[1].setLocation(x + SCREEN_WIDTH / 15, SCREEN_HEIGHT * 0.9);
	buttons[1].setSize(BUTTON_SIZE, BUTTON_SIZE);
	pauseRestButtonTexture.free();
	pauseClickedButtonTexture.free();

	// Set buttons[2] as Sound button
	LTexture soundRestButtonTexture, soundClickedButtonTexture;
	if (!soundRestButtonTexture.loadXPM(sound_button_rest_xpm)
		|| !soundClickedButtonTexture.loadXPM(sound_button_clicked_xpm))
		printf("Cannot load sound button!\n");

	buttons[2].setTexture(soundClickedButtonTexture, soundRestButtonTexture);
	buttons[2].setState(playingSoundOn);
	buttons[2].setLocation(x + SCREEN_WIDTH * 0.112, SCREEN_HEIGHT * 0.9);
	buttons[2].setSize(BUTTON_SIZE, BUTTON_SIZE);
	soundRestButtonTexture.free();
	soundClickedButtonTexture.free();


	// Set buttons[3] as Replay button
	LTexture replayButtonTexture;
	if (!replayButtonTexture.loadXPM(replay_button_xpm))
		printf("Cannot load replay button!\n");

	buttons[3].setTexture(replayButtonTexture, replayButtonTexture);
	buttons[3].setState(false);
	buttons[3].setLocation(SCREEN_WIDTH / 45, SCREEN_HEIGHT * 0.9);
	buttons[3].setSize(BUTTON_SIZE, BUTTON_SIZE);
	replayButtonTexture.free();

	// Set buttons[4] as Menu button
	LTexture menuButtonTexture;
	if (!menuButtonTexture.loadXPM(menu_button_xpm))
		printf("Cannot load menu button!\n");

	buttons[4].setTexture(menuButtonTexture, menuButtonTexture);
	buttons[4].setState(false);
	buttons[4].setLocation(SCREEN_WIDTH / 15, SCREEN_HEIGHT * 0.9);
	buttons[4].setSize(BUTTON_SIZE, BUTTON_SIZE);
	replayButtonTexture.free();
}

// Handle mouse event on buttons
void handleMouseEvent(SDL_Event& e, SButton buttons[], bool& pause, bool gameOver, bool& quit, bool& startOver, bool& goBack, bool& end)
{
	if (gameOver)
	{
		pause = true;
		buttons[0].setState(false);
		buttons[1].setState(true);

		if (buttons[0].buttonIsPressed(e))
		{
			quit = true;
			startOver = false;
		}
	}


	else if (e.type == SDL_KEYDOWN)
	{
		if (e.key.keysym.sym == SDLK_RETURN)
		{
			pause = !pause;
			buttons[0].setState(!pause);
			buttons[1].setState(pause);
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		// Get mouse's location
		int x, y;
		SDL_GetMouseState(&x, &y);
		
		// If user presses play
		if (buttons[0].buttonIsPressed(e))
		{
			if (gameOver)
			{
				end = true;
				return;
			}

			else
			{
				buttons[0].setState(!buttons[0].getState());
				pause = !buttons[0].getState();
				buttons[1].setState(pause);
			}
		}

		// If user presses Pause
		else if (buttons[1].buttonIsPressed(e) && !gameOver)
		{
			buttons[1].setState(!buttons[1].getState());
			pause = buttons[1].getState();
			buttons[0].setState(!pause);
		}

		// If user presses sound
		else if (buttons[2].buttonIsPressed(e))
		{
			buttons[2].setState(!buttons[2].getState());
			playingSoundOn = buttons[2].getState();
		}

		// If user presses replay
		else if (buttons[3].buttonIsPressed(e))
		{
			// If game is over, start the game over 
			if (gameOver)
				startOver = true;

			// If game is not over, display message to confirm demand
			else
				displayExitMessage(startOver, START_OVER);
			
			// If player want to start over
			if (startOver)
				quit = true;

			// If not, just pause the game 
			else
				pause = true;

		}

		// If user presses menu 
		else if (buttons[4].buttonIsPressed(e))
		{
			displayExitMessage(goBack, GO_BACK);
			
			// If player want to go back to main menu
			if (goBack)
			{
				gameOver = true;
				quit = true;
				startOver = false;
				return;
			}

			// If not, just pause the game
			else
				pause = true;
		}
	}
}


// Create button for start_up
void createOptionButton(SButton buttons[])
{
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);

	// Set buttons[0] as play button
	LTexture playButtonTexture, playButtonTexture2;
	playButtonTexture.loadXPM(play_button_xpm);
	playButtonTexture2.loadXPM(play_button2_xpm);

	buttons[0].setTexture(playButtonTexture, playButtonTexture2);
	buttons[0].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[0].setLocation(SCREEN_WIDTH / 3, SCREEN_WIDTH / 15);
	buttons[0].setState(true);
	playButtonTexture.free();
	playButtonTexture2.free();

	// Set buttons[1] as record button
	LTexture recordButtonTexture, recordButtonTexture2;
	recordButtonTexture.loadXPM(highscore_button_xpm);
	recordButtonTexture2.loadXPM(highscore_button2_xpm);

	buttons[1].setTexture(recordButtonTexture, recordButtonTexture2);
	buttons[1].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[1].setLocation(SCREEN_WIDTH / 3, SCREEN_WIDTH * 17 / 105);
	buttons[1].setState(true);
	recordButtonTexture.free();
	recordButtonTexture2.free();

	// Set buttons[2] as option button
	LTexture optionButtonTexture, optionButtonTexture2;
	optionButtonTexture.loadXPM(option_button_xpm);
	optionButtonTexture2.loadXPM(option_button2_xpm);

	buttons[2].setTexture(optionButtonTexture, optionButtonTexture2);
	buttons[2].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[2].setLocation(SCREEN_WIDTH / 3, SCREEN_WIDTH * 9 / 35); 
	buttons[2].setState(true);
	optionButtonTexture.free();
	optionButtonTexture2.free();
	

	// Set buttons[3] as exit button
	LTexture exitButtonTexture, exitButtonTexture2;
	exitButtonTexture.loadXPM(exit_button_xpm);
	exitButtonTexture2.loadXPM(exit_button2_xpm);

	buttons[3].setTexture(exitButtonTexture, exitButtonTexture2);
	buttons[3].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[3].setLocation(SCREEN_WIDTH / 3, SCREEN_WIDTH * 37 / 105);
	buttons[3].setState(true);
	exitButtonTexture.free();
	exitButtonTexture2.free();
}

// Handle mouse event for start_up
void handleEventStartup(SDL_Event& e, SButton buttons[], bool& jumpToNextScene, bool& exit, bool& goBack)
{
	// Change state for each button when mouse is over it
	for (int i = 0; i < 4; i++)
		buttons[i].setState(!buttons[i].mouseOnButton(e));
	// Render them
	SDL_RenderClear(gRenderer);
	gTheme.render(0, 0);
	renderButton(buttons, 4);

	// Handle event when Play button is pressed
	if (buttons[0].buttonIsPressed(e))
	{
		// Play music
		if (Mix_PlayingMusic() == 0 && setupSoundOn)
			Mix_PlayMusic(gThemeMusic, -1);

		// Pace 
		int pace = 50;

		for (int i = 0; i <= 300 / pace; i++)
		{
			// Draw background
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_Rect background1 = { buttons[1].getX() + buttons[1].getWidth() - i * pace, buttons[1].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_Rect background2 = { buttons[2].getX() + buttons[2].getWidth() - i * pace, buttons[2].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_Rect background3 = { buttons[3].getX() + buttons[3].getWidth() - i * pace, buttons[3].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_RenderFillRect(gRenderer, &background1);
			SDL_RenderFillRect(gRenderer, &background2);
			SDL_RenderFillRect(gRenderer, &background3);

			// Draw theme
			gTheme.render(0, 0);

			// Draw buttons at new position
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
			buttons[1].render(buttons[1].getX() - i * pace, buttons[1].getY());
			buttons[2].render(buttons[2].getX() - i * pace, buttons[2].getY());
			buttons[3].render(buttons[3].getX() - i * pace, buttons[3].getY());


			// Draw play button
			buttons[0].render(buttons[0].getX(), buttons[0].getY());
			SDL_Delay(150);
			SDL_RenderPresent(gRenderer);
			SDL_RenderClear(gRenderer);
		}

		// Translate play button and render it
		buttons[0].translateButton(DOWN, 150);
		buttons[0].translateButton(LEFT, 200);


		// Set other button far away so that they cant be seen
		buttons[1].setLocation(1000, 500);
		buttons[2].setLocation(1000, 500);
		buttons[3].setLocation(1000, 500);
		
		SDL_Delay(500);

		// Create easy, normal and hard buttons
		SButton levelButtons[5];
		createLevelButton(levelButtons, buttons[0].getX(), buttons[0].getY());

		while (!jumpToNextScene && !exit && !goBack)
		{
			handleLevelButton(levelButtons, jumpToNextScene, exit, goBack);
		}


		// Stop music if go back to main menu or jump to next screen
		if (goBack || jumpToNextScene)
			Mix_HaltMusic();
	}

	// Handle event when Record button is pressed
	else if (buttons[1].buttonIsPressed(e))
	{

		// Pace 
		int pace = 50;


		for (int i = 0; i <= 300 / pace; i++)
		{
			// Draw background
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_Rect background1 = { buttons[0].getX() + buttons[0].getWidth() - i * pace, buttons[0].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_Rect background2 = { buttons[2].getX() + buttons[2].getWidth() - i * pace, buttons[2].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_Rect background3 = { buttons[3].getX() + buttons[3].getWidth() - i * pace, buttons[3].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_RenderFillRect(gRenderer, &background1);
			SDL_RenderFillRect(gRenderer, &background2);
			SDL_RenderFillRect(gRenderer, &background3);

			// Draw theme
			gTheme.render(0, 0);

			// Draw buttons at new position
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
			buttons[0].render(buttons[0].getX() - i * pace, buttons[0].getY());
			buttons[2].render(buttons[2].getX() - i * pace, buttons[2].getY());
			buttons[3].render(buttons[3].getX() - i * pace, buttons[3].getY());

			// Draw target button
			buttons[1].render(buttons[1].getX(), buttons[1].getY());
			SDL_Delay(150);
			SDL_RenderPresent(gRenderer);
			SDL_RenderClear(gRenderer);
		}

		// Translate record button
		buttons[1].translateButton(DOWN, 50);
		buttons[1].translateButton(LEFT, 250);

		// Set other button location so that they cant be see
		buttons[0].setLocation(1000, 500);
		buttons[2].setLocation(1000, 500);
		buttons[3].setLocation(1000, 500);

		SDL_Delay(500);


		// Render the go back button and handle mouse event on go back button
		handleGoback(exit, goBack, buttons[1].getX(), buttons[1].getY());

	}

	// Handle event when Option button is pressed
	else if (buttons[2].buttonIsPressed(e))
	{

		// Pace 
		int pace = 50;

		for (int i = 0; i <= 300 / pace; i++)
		{
			// Draw background
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_Rect background1 = { buttons[0].getX() + buttons[0].getWidth() - i * pace, buttons[0].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_Rect background2 = { buttons[1].getX() + buttons[1].getWidth() - i * pace, buttons[1].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_Rect background3 = { buttons[3].getX() + buttons[3].getWidth() - i * pace, buttons[3].getY(),
				i * pace, buttons[1].getHeight() };
			SDL_RenderFillRect(gRenderer, &background1);
			SDL_RenderFillRect(gRenderer, &background2);
			SDL_RenderFillRect(gRenderer, &background3);

			// Draw theme
			gTheme.render(0, 0);

			// Draw buttons at new position
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
			buttons[0].render(buttons[0].getX() - i * pace, buttons[0].getY());
			buttons[1].render(buttons[1].getX() - i * pace, buttons[1].getY());
			buttons[3].render(buttons[3].getX() - i * pace, buttons[3].getY());

			// Draw button
			buttons[2].render(buttons[2].getX(), buttons[2].getY());
			SDL_Delay(150);
			SDL_RenderPresent(gRenderer);
			SDL_RenderClear(gRenderer);
		}

		// Translate option button
		buttons[2].translateButton(UP, 50);
		buttons[2].translateButton(LEFT, 200);

		// Set other button location so that they cant be see
		buttons[0].setLocation(1000, 500);
		buttons[1].setLocation(1000, 500);
		buttons[3].setLocation(1000, 500);

		SDL_Delay(500);


		// Handle option buttons
		handleOption(exit, goBack, buttons[2].getX(), buttons[2].getY());
	}

	// Handle event when Exit button is pressed
	else if (buttons[3].buttonIsPressed(e))
		displayExitMessage(exit, EXIT);

}



// Create level buttons
// @param buttons[] - array of level buttons
// @param x, y - positions of the play button (set as buttons[0])
void createLevelButton(SButton buttons[], int x, int y)
{
	// Set buttons[0] as play button
	LTexture playButtonTexture, playButtonTexture2;
	playButtonTexture.loadXPM(play_button_xpm);
	playButtonTexture2.loadXPM(play_button2_xpm);

	buttons[0].setTexture(playButtonTexture, playButtonTexture2);
	buttons[0].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[0].setLocation(x, y);
	buttons[0].setState(false);
	playButtonTexture.free();
	playButtonTexture2.free();

	// Set buttons[1] as easy button
	LTexture easyButtonTexture, easyButtonTexture2;
	easyButtonTexture.loadXPM(easy_button_xpm);
	easyButtonTexture2.loadXPM(easy_button2_xpm);

	buttons[1].setTexture(easyButtonTexture, easyButtonTexture2);
	buttons[1].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[1].setLocation(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT / 7);
	buttons[1].setState(true);
	easyButtonTexture.free();
	easyButtonTexture2.free();

	// Set buttons[2] as normal button
	LTexture normalButtonTexture, normalButtonTexture2;
	normalButtonTexture.loadXPM(normal_button_xpm);
	normalButtonTexture2.loadXPM(normal_button2_xpm);

	buttons[2].setTexture(normalButtonTexture, normalButtonTexture2);
	buttons[2].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[2].setLocation(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.37);
	buttons[2].setState(true);
	normalButtonTexture.free();
	normalButtonTexture2.free();

	// Set buttons[3] as hard button
	LTexture hardButtonTexture, hardButtonTexture2;
	hardButtonTexture.loadXPM(hard_button_xpm);
	hardButtonTexture2.loadXPM(hard_button2_xpm);

	buttons[3].setTexture(hardButtonTexture, hardButtonTexture2);
	buttons[3].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[3].setLocation(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 7 / 12);
	buttons[3].setState(true);
	hardButtonTexture.free();
	hardButtonTexture2.free();

	// Set buttons[4] as goback button
	LTexture gobackButtonTexture, gobackButtonTexture2;
	gobackButtonTexture.loadXPM(goback_button_xpm);
	gobackButtonTexture2.loadXPM(goback_button2_xpm);

	buttons[4].setTexture(gobackButtonTexture, gobackButtonTexture2);
	buttons[4].setSize(150, 100);
	buttons[4].setLocation(0, 0);
	buttons[4].setState(true);
	gobackButtonTexture.free();
	gobackButtonTexture2.free();
}

// Handle level buttons
void handleLevelButton(SButton levelButtons[], bool& jumpToNextScene, bool& exit, bool& goBack)
{
	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			// Check if player demand quit
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			{
				displayExitMessage(exit, EXIT);

				if (exit)
				{
					quit = true;
					break;
				}
			}

			// Change state when mouse is over button
			for (int i = 1; i < 5; i++)
			{
				levelButtons[i].setState(!levelButtons[i].mouseOnButton(e));
			}

			// Clear renderer
			SDL_Color red = { 0xFF, 0, 0 };
			SDL_RenderClear(gRenderer);

			// Draw theme
			gPlay.render(0, 0);

			// Draw texture
			LTexture easyTexture;

			if (levelButtons[1].mouseOnButton(e))
			{
				easyTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 15),
					"Snake's Speed: Slow. Do not hit bombs and monster!", red);
				easyTexture.render(SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 0.9);
			}

			else if (levelButtons[2].mouseOnButton(e))
			{
				easyTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 15),
					"Snake's Speed: Normal. Do not hit bombs and monster!", red);
				easyTexture.render(SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 0.9);
			}


			else if (levelButtons[3].mouseOnButton(e))
			{
				easyTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 15),
					"Snake's Speed: Fast. Do not hit bombs and monster!", red);
				easyTexture.render(SCREEN_WIDTH * 0.2, SCREEN_HEIGHT * 0.9);
			}

			else if (levelButtons[4].mouseOnButton(e))
			{
				easyTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 15),
					"Go Back To The Main Menu", red);
				easyTexture.render(SCREEN_WIDTH * 0.35, SCREEN_HEIGHT * 0.9);
			}

			else
			{
				easyTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 15),
					"Choose A Game Level You Wish To Play", red);
				easyTexture.render(SCREEN_WIDTH * 0.3, SCREEN_HEIGHT * 0.9);
			}

			// Render button
			renderButton(levelButtons, 5);
			SDL_RenderPresent(gRenderer);
			easyTexture.free();

			// Handle button
			if (levelButtons[1].buttonIsPressed(e))
			{
				GAME_LEVEL = EASY;
				jumpToNextScene = true;
				quit = true;

				// Play music
				if (setupSoundOn)
					Mix_PlayChannel(-1, gButtonSound, 0);

				break;
			}

			else if (levelButtons[2].buttonIsPressed(e))
			{
				GAME_LEVEL = NORMAL;
				jumpToNextScene = true;
				quit = true;

				// Play music
				if (setupSoundOn)
					Mix_PlayChannel(-1, gButtonSound, 0);

				break;
			}

			else if (levelButtons[3].buttonIsPressed(e))
			{
				GAME_LEVEL = HARD;
				jumpToNextScene = true;
				quit = true;

				// Play music
				if (setupSoundOn)
					Mix_PlayChannel(-1, gButtonSound, 0);

				break;
			}

			else if (levelButtons[4].buttonIsPressed(e))
			{
				jumpToNextScene = false;
				quit = true;
				goBack = true;
				break;
			}
		}
	}
}


// Handle goback button
// @param x, y- positions of the record button
void handleGoback(bool& exit, bool& goback, int x, int y)
{
	// Read score
	readScore();

	// Create array of 2 buttons
	SButton buttons[2];

	// Set buttons[0] as the Record button
	LTexture recordButtonTexture, recordButtonTexture2;
	recordButtonTexture.loadXPM(highscore_button_xpm);
	recordButtonTexture2.loadXPM(highscore_button2_xpm);
	buttons[0].setTexture(recordButtonTexture2,recordButtonTexture);
	buttons[0].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[0].setLocation(x, y);
	buttons[0].setState(false);
	recordButtonTexture.free();
	recordButtonTexture2.free();

	// Set buttons[1] as the Goback button
	LTexture gobackButtonTexture, gobackButtonTexture2;
	gobackButtonTexture.loadXPM(goback_button_xpm);
	gobackButtonTexture2.loadXPM(goback_button2_xpm);
	buttons[1].setTexture(gobackButtonTexture2, gobackButtonTexture);
	buttons[1].setSize(150, 100);
	buttons[1].setLocation(0, 0);
	buttons[1].setState(true);
	gobackButtonTexture.free();
	gobackButtonTexture2.free();

	// Poll for a new event
	SDL_Event e;
	while (!exit && !goback)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			// Check if player wanna quit
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			{
				displayExitMessage(exit, EXIT);
				if (exit)
					break;
			}


			// Change state when mouse is over Goback button
			buttons[1].setState(buttons[1].mouseOnButton(e));

			// Check if player presses goback
			if (buttons[1].buttonIsPressed(e))
			{
				goback = true;
				break;
			}

			// Clear renderer
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0xFF);
			SDL_RenderClear(gRenderer);

			// Draw theme and buttons
			gRecord.render(0, 0);
			renderButton(buttons, 2);

			// Render high score
			SDL_Color red = { 0xFF, 0, 0 };
			SDL_Color blue = { 0, 0, 0xFF };
			SDL_Color black = { 0, 0, 0 };

			LTexture lastPlayedGameTexture; 
			lastPlayedGameTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 40),
				"LAST PLAYED GAMES", blue); 
			lastPlayedGameTexture.render(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.05);

			LTexture titleTexture;
			titleTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				"     Level            Score", red);
			titleTexture.render(SCREEN_WIDTH * 0.53, SCREEN_HEIGHT * 0.2);
			
			LTexture firstTexture;
			firstTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				"1." , red);
			firstTexture.render(SCREEN_WIDTH * 0.45, SCREEN_HEIGHT * 0.2 + 60);

			LTexture secondTexture;
			secondTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				"2.", red);
			secondTexture.render(SCREEN_WIDTH * 0.45, SCREEN_HEIGHT * 0.2 + 120);
			
			LTexture thirdTexture;
			thirdTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				"3.", red);
			thirdTexture.render(SCREEN_WIDTH * 0.45, SCREEN_HEIGHT * 0.2 + 180);

			LTexture highscoreTexture;
			highscoreTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				"High Score", red);
			highscoreTexture.render(SCREEN_WIDTH * 0.35, SCREEN_HEIGHT * 0.2 + 240);

			LTexture level_first;
			level_first.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				LEVEL_FIRST, black);
			level_first.render(SCREEN_WIDTH * 0.6, SCREEN_HEIGHT * 0.2 + 60);

			LTexture first;
			first.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				to_string(FIRST), black);
			first.render(SCREEN_WIDTH * 0.85, SCREEN_HEIGHT * 0.2 + 60);

			LTexture level_second;
			level_second.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				LEVEL_SECOND, black);
			level_second.render(SCREEN_WIDTH * 0.6, SCREEN_HEIGHT * 0.2 + 120);

			LTexture second;
			second.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				to_string(SECOND), black);
			second.render(SCREEN_WIDTH * 0.85, SCREEN_HEIGHT * 0.2 + 120);

			LTexture level_third;
			level_third.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				LEVEL_THIRD, black);
			level_third.render(SCREEN_WIDTH * 0.6, SCREEN_HEIGHT * 0.2 + 180);

			LTexture third;
			third.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
				to_string(THIRD), black);
			third.render(SCREEN_WIDTH * 0.85, SCREEN_HEIGHT * 0.2 + 180);

			LTexture level_highscore;
			level_highscore.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 30),
				LEVEL_HIGHSCORE, black);
			level_highscore.render(SCREEN_WIDTH * 0.6, SCREEN_HEIGHT * 0.2 + 240);

			LTexture highscore;
			highscore.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 30),
				to_string(HIGHSCORE), black);
			highscore.render(SCREEN_WIDTH * 0.85, SCREEN_HEIGHT * 0.2 + 240);

			// Free textures
			lastPlayedGameTexture.free();
			titleTexture.free();
			firstTexture.free();
			secondTexture.free();
			thirdTexture.free();
			highscoreTexture.free();
			level_first.free();
			first.free();

			// Update screen
			SDL_RenderPresent(gRenderer);
		}
	}
}

// Handle option buttons
// @param x, y - position of the Option button after translate
void handleOption(bool& exit, bool& goBack, int x, int y)
{
	// Create array of 5 buttons
	SButton buttons[5];

	// Set buttons[0] as the Option button
	LTexture optionButtonTexture, optionButtonTexture2;
	optionButtonTexture.loadXPM(option_button_xpm);
	optionButtonTexture2.loadXPM(option_button2_xpm);
	buttons[0].setTexture(optionButtonTexture2, optionButtonTexture);
	buttons[0].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[0].setLocation(x, y);
	buttons[0].setState(false);
	optionButtonTexture.free();
	optionButtonTexture2.free();

	// Set buttons[1] as the go back button
	LTexture gobackButtonTexture, gobackButtonTexture2;
	gobackButtonTexture.loadXPM(goback_button_xpm);
	gobackButtonTexture2.loadXPM(goback_button2_xpm);
	buttons[1].setTexture(gobackButtonTexture2, gobackButtonTexture);
	buttons[1].setSize(OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT);
	buttons[1].setLocation(0, 0);
	buttons[1].setState(false);
	gobackButtonTexture.free();
	gobackButtonTexture2.free();

	// Set buttons[2] as the playing sound sound button
	LTexture playingSoundButtonTexture, playingSoundButtonTexture2, playingSoundButtonTexture3;
	playingSoundButtonTexture.loadXPM(button_mouse_over_xpm);
	//playingSoundButtonTexture2.loadXPM(button2_xpm);
	playingSoundButtonTexture2.loadFromFile("resources/button.bmp");
	playingSoundButtonTexture3.loadXPM(button_clicked_xpm);
	buttons[2].setTexture(playingSoundButtonTexture, playingSoundButtonTexture2);
	buttons[2].setSize(SQUARE_SIZE, SQUARE_SIZE);
	buttons[2].setLocation(SCREEN_WIDTH * 0.75, SCREEN_HEIGHT * 0.15);
	buttons[2].setState(false);


	// Set buttons[3] as the menu sound sound button
	LTexture menuSoundButtonTexture, menuSoundButtonTexture2, menuSoundButtonTexture3;
	menuSoundButtonTexture.loadXPM(button_mouse_over_xpm);
	//menuSoundButtonTexture2.loadXPM(button2_xpm);
	menuSoundButtonTexture2.loadFromFile("resources/button.bmp");
	menuSoundButtonTexture3.loadXPM(button_clicked_xpm);
	buttons[3].setTexture(menuSoundButtonTexture, menuSoundButtonTexture2);
	buttons[3].setSize(SQUARE_SIZE, SQUARE_SIZE);
	buttons[3].setLocation(SCREEN_WIDTH * 0.75, SCREEN_HEIGHT * 0.3);
	buttons[3].setState(false);

	// Set buttons[4] as the volume button
	LTexture volumeButtonTexture, volumeButtonTexture2;
	volumeButtonTexture.loadXPM(volume_button_clicked_xpm);
	volumeButtonTexture2.loadXPM(volume_button_xpm);
	buttons[4].setTexture(volumeButtonTexture, volumeButtonTexture2);
	buttons[4].setSize(10, 20);
	buttons[4].setState(false);

	// Bound for volume bar
	int leftBound = SCREEN_WIDTH * 0.65;
	int rightBound = SCREEN_WIDTH * 0.85;
	
	// Reserve position corresponding to gVolume
	int oldX = gVolume * (rightBound - buttons[4].getWidth() - leftBound) / MIX_MAX_VOLUME + leftBound;
	int oldY = SCREEN_HEIGHT * 0.45;
	buttons[4].setLocation(oldX, oldY);

	// Flag to indicate volume button is pressed
	bool isPressed = false;

	// Enable text input
	SDL_StartTextInput();
	// Input text
	SDL_Color black = { 0, 0, 0 };
	gPlayerTextTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
		gName, black);

	// Flag to render text
	bool renderText = false;

	// Text focus flag
	bool textFocus = false;

	// Poll event for a new event
	SDL_Event e;
	while (!exit && !goBack)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			// If player request quit
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			{
				// Display exit message
				displayExitMessage(exit, EXIT);

				if (exit)
					break;
			}


			// Change state when mouse is over
			buttons[1].setState(buttons[1].mouseOnButton(e));

			if (!playingSoundOn)
				buttons[2].setState(buttons[2].mouseOnButton(e));

			if (!setupSoundOn)
				buttons[3].setState(buttons[3].mouseOnButton(e));

			if (!isPressed)
				buttons[4].setState(buttons[4].mouseOnButton(e));

			// If player presses go Back
			if (buttons[1].buttonIsPressed(e))
			{
				goBack = true;
				break;
			}

			// If playing sound button is pressed
			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT
				&& e.button.x >= buttons[2].getX() && e.button.x <= buttons[2].getX() + buttons[2].getWidth()
				&& e.button.y >= buttons[2].getY() && e.button.y <= buttons[2].getY() + buttons[2].getHeight())
				playingSoundOn = !playingSoundOn;

			// If menu sound button is pressed
			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT
				&& e.button.x >= buttons[3].getX() && e.button.x <= buttons[3].getX() + buttons[3].getWidth()
				&& e.button.y >= buttons[3].getY() && e.button.y <= buttons[3].getY() + buttons[3].getHeight())
				setupSoundOn = !setupSoundOn;

			// If volume modulation button is presses
			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT
				&& e.button.x >= buttons[4].getX() && e.button.x <= buttons[4].getX() + buttons[4].getWidth()
				&& e.button.y >= buttons[4].getY() && e.button.y <= buttons[4].getY() + buttons[4].getHeight())
			{
				buttons[4].setState(true);
				isPressed = true;
			}

			else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
			{
				buttons[4].setState(false);
				isPressed = false;
			}

			else if (isPressed && e.type == SDL_MOUSEMOTION)
			{
				int x, y;
				SDL_GetMouseState(&x, &y);

				if (x < leftBound)
					x = leftBound;
				else if (x > rightBound - buttons[4].getWidth())
					x = rightBound - buttons[4].getWidth();

				buttons[4].setLocation(x, buttons[4].getY());

				if (buttons[4].getX() == leftBound)
				{
					playingSoundOn = false;
					setupSoundOn = false;
				}
				else
					playingSoundOn = true;


			}

			else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT
				&& e.button.x >= leftBound && e.button.x <= rightBound - buttons[4].getWidth()
				&& e.button.y >= buttons[4].getY() && e.button.y <= buttons[4].getY() + buttons[4].getHeight())
			{
				buttons[4].setLocation(e.button.x, buttons[4].getY());
				if (buttons[4].getX() == leftBound)
				{
					playingSoundOn = false;
					setupSoundOn = false;
				}
				else
					playingSoundOn = true;

			}

			// Special key inputs
			else if (e.type == SDL_KEYDOWN && textFocus)
			{
				// Handle backspace
				if (e.key.keysym.sym == SDLK_BACKSPACE && gName.length() > 0)
				{
					gName.pop_back();
					renderText = true;
				}

				// Handle paste
				else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
				{
					gName = SDL_GetClipboardText();
					renderText = true;
				}
			}

			// Special text input event
			else if (e.type == SDL_TEXTINPUT && textFocus)
			{
				// Not pasting
				if (!((e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL) && gName.length() < 12)
				{
					// Append character
					gName += e.text.text;
					renderText = true;
				}
			}

			// Check if player click on enter_text area
			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
			{
				int x, y;
				SDL_GetMouseState(&x, &y);
				if (x >= SCREEN_WIDTH * 0.5 && x <= SCREEN_WIDTH && y >= SCREEN_HEIGHT * 0.55 && y <= SCREEN_HEIGHT * 0.65)
					textFocus = true;

				else
					textFocus = false;
			}

		}

		// Render input text
		if (renderText)
		{
			// If input text is not empty
			if (gName != "")
			{
				gPlayerTextTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
					gName, black);
			}

			else
			{
				gPlayerTextTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
					"", black);

			}
		}

		// Handle volume modulation
		gVolume = (buttons[4].getX() - leftBound) * MIX_MAX_VOLUME / (rightBound - buttons[4].getWidth() - leftBound);
		Mix_Volume(-1, gVolume);
		Mix_VolumeMusic(gVolume);
	

		// Clear renderer
		SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xFF);
		SDL_RenderClear(gRenderer);

		// Render buttons
		renderButton(buttons, 2);
		if (!playingSoundOn)
			buttons[2].render(buttons[2].getX(), buttons[2].getY());
		else
			playingSoundButtonTexture3.render(buttons[2].getX(), buttons[2].getY());

		if (!setupSoundOn)
			buttons[3].render(buttons[3].getX(), buttons[3].getY());
		else
			menuSoundButtonTexture3.render(buttons[3].getX(), buttons[3].getY());

		// Render text
		gPlayerTextTexture.render(SCREEN_WIDTH * 0.7, SCREEN_HEIGHT * 0.6);

		LTexture playingSoundTexture;

		playingSoundTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
			"Playing Sound ", black);
		playingSoundTexture.render(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.15);


		LTexture menuSoundTexture;
		menuSoundTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
			"Menu Sound ", black);
		menuSoundTexture.render(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.3);

		LTexture volumeTexture;
		volumeTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
			"Volume ", black);
		volumeTexture.render(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.45);

		LTexture nameTexture;
		nameTexture.loadFromRenderedText(TTF_OpenFont("resources/DejaVuSansMono-Bold.ttf", 20),
			"Enter Name: ", black);
		nameTexture.render(SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.6);

		// Draw volume bar
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xff);
		SDL_RenderDrawLine(gRenderer, leftBound, SCREEN_HEIGHT * 0.47, rightBound, SCREEN_HEIGHT * 0.47);

		// Draw volume button
		buttons[4].render(buttons[4].getX(), buttons[4].getY());

		// Draw line under text
		SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH * 0.68, SCREEN_HEIGHT * 0.6 + 20, 
			gPlayerTextTexture.getWidth() + SCREEN_WIDTH * 0.72, SCREEN_HEIGHT * 0.6 + 20);

		// Update screen
		SDL_RenderPresent(gRenderer);

		// Clear text texture
		playingSoundTexture.free();
		menuSoundTexture.free();
		volumeTexture.free();
		nameTexture.free();

	}

	// Disable text input
	SDL_StopTextInput();

	// Clear texture
	playingSoundButtonTexture3.free();
	menuSoundButtonTexture3.free();
	playingSoundButtonTexture.free();
	playingSoundButtonTexture2.free();
	menuSoundButtonTexture.free();
	menuSoundButtonTexture2.free();
	volumeButtonTexture.free();
	volumeButtonTexture2.free();
}


// Display exit message
void displayExitMessage(bool& exit, DEMAND_TYPE demand)
{
	// Pause music when message shows
	Mix_PauseMusic();

	SDL_Window* exitWindow = NULL;
	SDL_Renderer* exitRenderer = NULL;
	
	// Create exit window
	exitWindow = SDL_CreateWindow("Demand Confirmation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2,
		SDL_WINDOW_SHOWN);

	// Create renderer for exit window
	exitRenderer = SDL_CreateRenderer(exitWindow, -1, SDL_RENDERER_ACCELERATED);

	int ID = SDL_GetWindowID(exitWindow);
	


	// Select message
	SDL_Surface* surface = NULL;
	if (demand == EXIT)
		surface = IMG_ReadXPMFromArray(quit_message_xpm);
	else if (demand == GO_BACK)
		surface = IMG_ReadXPMFromArray(go_back_message_xpm);
	else if (demand == CHANGE_LEVEL)
		surface = IMG_ReadXPMFromArray(change_level_message_xpm);
	else if (demand == START_OVER)
		surface = IMG_ReadXPMFromArray(start_over_message_xpm);

	if (!surface)
		printf("Cannot open either quit_message, go_back_message, change_level_message, or start_over_message\n");

	SDL_Texture* texture = SDL_CreateTextureFromSurface(exitRenderer, surface);
	SDL_Rect quad = { 0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4 };


	// Draw YES buttons
	SDL_Surface* yes_surface = IMG_ReadXPMFromArray(yes_pressed_button_xpm);
	if (!yes_surface)
		printf("Cannot open yes_pressed_button_xpm.\n");
	SDL_Surface* yes_rest_surface = IMG_ReadXPMFromArray(yes_rest_button_xpm);
	if (!yes_rest_surface)
		printf("Cannot open yes_rest_button_xpm.\n");
	SDL_SetColorKey(yes_surface, SDL_TRUE, SDL_MapRGB(yes_surface->format, 0, 0xFF, 0xFF));
	SDL_SetColorKey(yes_rest_surface, SDL_TRUE, SDL_MapRGB(yes_surface->format, 0, 0xFF, 0xFF));
	SDL_Texture* yes_texture = SDL_CreateTextureFromSurface(exitRenderer, yes_surface);
	SDL_Texture* yes_rest_texture = SDL_CreateTextureFromSurface(exitRenderer, yes_rest_surface);
	SDL_Rect yes_quad = { (SCREEN_WIDTH / 2 - 2 * yes_surface->w) / 3, SCREEN_HEIGHT / 4 + (SCREEN_HEIGHT / 4 - yes_surface->h)/2,
		yes_surface->w, yes_surface->h };

	// Draw NO buttons
	SDL_Surface* no_surface = IMG_ReadXPMFromArray(no_pressed_button_xpm);
	if (!no_surface)
		printf("Cannot open no_pressed_button_xpm.\n");
	SDL_Surface* no_rest_surface = IMG_ReadXPMFromArray(no_rest_button_xpm);
	if (!no_rest_surface)
		printf("Cannot open no_rest_button_xpm.\n");
	SDL_SetColorKey(no_surface, SDL_TRUE, SDL_MapRGB(no_surface->format, 0, 0xFF, 0xFF));
	SDL_SetColorKey(no_rest_surface, SDL_TRUE, SDL_MapRGB(no_surface->format, 0, 0xFF, 0xFF));
	SDL_Texture* no_texture = SDL_CreateTextureFromSurface(exitRenderer, no_surface);
	SDL_Texture* no_rest_texture = SDL_CreateTextureFromSurface(exitRenderer, no_rest_surface);
	SDL_Rect no_quad = { 2 * yes_quad.x + yes_quad.w, yes_quad.y, no_surface->w, no_surface->h };

	// Get buttons state
	bool yes_button_isFocus = false;
	bool no_button_isFocus = false;

	// Event handler
	SDL_Event e;
	
	// Main loop flag
	bool endLoop = false;

	while (!endLoop)
	{
		// Clear renderer
		SDL_SetRenderDrawColor(exitRenderer, 0xC2, 0xD5, 0xD5, 0xFF);
		SDL_RenderClear(exitRenderer);



		while (SDL_PollEvent(&e) != 0)
		{
			// Get mouse positions
			int x, y;
			SDL_GetMouseState(&x, &y);
			
			// Check if mouse is over yes button
			if (x > yes_quad.x && x < yes_quad.x + yes_quad.w && y > yes_quad.y && y < yes_quad.y + yes_quad.h)
				yes_button_isFocus = true;
			else
				yes_button_isFocus = false;

			// Check if mouse is over no button
			if (x > no_quad.x && x < no_quad.x + no_quad.w && y > no_quad.y && y < no_quad.y + no_quad.h)
				no_button_isFocus = true;
			else
				no_button_isFocus = false;

			// Check if player hit close
			if (e.type == SDL_WINDOWEVENT && e.window.windowID == ID)
			{
				if (e.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					exit = false;
					endLoop = true;
					break;
				}
			}

			// If player hit yes button
			else if (e.type == SDL_MOUSEBUTTONUP && yes_button_isFocus)
			{
				exit = true;
				endLoop = true;
				break;
			}

			// If player hit no button
			else if (e.type == SDL_MOUSEBUTTONUP && no_button_isFocus)
			{
				exit = false;
				endLoop = true;
				break;
			}
			

		}

		// Render quit message
		SDL_RenderCopy(exitRenderer, texture, NULL, &quad);

		// Render buttons
		if (yes_button_isFocus)
			SDL_RenderCopy(exitRenderer, yes_texture, NULL, &yes_quad);
		else
			SDL_RenderCopy(exitRenderer, yes_rest_texture, NULL, &yes_quad);

		if (no_button_isFocus)
			SDL_RenderCopy(exitRenderer, no_texture, NULL, &no_quad);
		else
			SDL_RenderCopy(exitRenderer, no_rest_texture, NULL, &no_quad);
		
		// Update screen
		SDL_RenderPresent(exitRenderer);

	}

	// Clear surface and texture
	SDL_DestroyTexture(texture);
	texture = NULL;
	SDL_FreeSurface(surface);
	surface = NULL;

	SDL_DestroyTexture(yes_rest_texture);
	yes_rest_texture = NULL;
	SDL_FreeSurface(yes_rest_surface);
	yes_rest_surface = NULL;

	SDL_DestroyTexture(no_texture);
	no_texture = NULL;
	SDL_FreeSurface(no_surface);
	no_surface = NULL;

	SDL_DestroyTexture(no_rest_texture);
	no_rest_texture = NULL;
	SDL_FreeSurface(no_rest_surface);
	no_rest_surface = NULL;

	// Destroy window
	SDL_DestroyWindow(exitWindow);
	exitWindow = NULL;
	SDL_DestroyRenderer(exitRenderer);
	exitRenderer = NULL;
	
	// Resume the music
	Mix_ResumeMusic();
}

// Output score to text file
// Output score is written in the following format
// LEVEL_HIGHSCORE highscore LEVEL_FIRST first LEVEL_SECOND second LEVEL_THIRD third;
void outputScore(int score)
{
	ifstream fin;
	ofstream fout;

	fin.open("resources/record.txt");
	int level_highscore, level_first, level_second, level_third;
	
	fin >> level_highscore >> HIGHSCORE>> level_second >> SECOND >> level_third >> THIRD;
	FIRST = score;
	level_first = GAME_LEVEL;
	
	// Check if there is new high score
	if (HIGHSCORE < FIRST)
	{
		HIGHSCORE = FIRST;
		level_highscore = level_first;
	}

	// Close fin
	fin.close();

	// Open fout
	fout.open("resources/record.txt");
	fout << level_highscore << " " << HIGHSCORE << " " << level_first << " " << FIRST << " " << level_second << " " 
		<< SECOND << " " << level_third << " " << THIRD;

	// Close fout
	fout.close();

	// Convert level to string
	LEVEL_HIGHSCORE = convertLevel(level_highscore);
	LEVEL_FIRST = convertLevel(level_first);
	LEVEL_SECOND = convertLevel(level_second);
	LEVEL_THIRD = convertLevel(level_third);
}

// Convert numeric level flags to strings such as EASY, NORMAL, and HARD
string convertLevel(int level)
{
	switch(level)
	{
	case 0:
		return "EASY";
	case 1:
		return "NORMAL";
	case 2: 
		return "HARD";
	}
}

// Read score in text file
void readScore()
{
	ifstream fin;
	fin.open("resources/record.txt");
	int level_highscore, level_first, level_second, level_third;

	fin >> level_highscore >> HIGHSCORE >> level_first >> FIRST >> level_second >> SECOND >> level_third >> THIRD;
	fin.close();

	// Convert level to string
	LEVEL_HIGHSCORE = convertLevel(level_highscore);
	LEVEL_FIRST = convertLevel(level_first);
	LEVEL_SECOND = convertLevel(level_second);
	LEVEL_THIRD = convertLevel(level_third);
}