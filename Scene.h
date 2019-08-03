#ifndef SCENE_H
#define SCENE_H

#include "global.h"
#include "Snake.h"
#include <vector>

// Start up game
void start_up(bool& exit);

// Create option button
void createOptionButton(SButton buttons[]);

// Create easy, normal, and hard button
void createLevelButton(SButton buttons[], int x, int y);

// Handle level buttons
void handleLevelButton(SButton buttons[], bool& jumpToNextScene, bool& exit, bool& goBack);

// Handle goback button when player presses Record
void handleGoback(bool& exit, bool& goBack, int x, int y);

// Handle buttons when player presses Option button
void handleOption(bool& exit, bool& goBack, int x, int y);

// Handle mouse event on buttons in start_up
void handleEventStartup(SDL_Event& e, SButton buttons[], bool& jumpToNextScene, bool& exit, bool& goBack);

// Create buttons
void createButton(SButton buttons[]);

// Playing game
bool play_game(bool& exit, bool& end);

// Draw Screen
void drawScreen();

// Render player score
void renderScore(bool& quit, const Snake& snake);

// Render game over message
void renderGameOver();

// Render button
void renderButton(SButton buttons[], int n);

// Handle mouse event on buttons
void handleMouseEvent(SDL_Event& e, SButton buttons[], bool& pause, bool gameOver, bool& quit, 
	bool& startOver, bool& goBack, bool& end);

// End game
void end_game();

// Create monster
void createMonster(Monster& monster, const Snake& snake);

// Create food
void createFood(Square& food);

// Create bombs
void createBomb(std::vector<Square>& bombs, const Square& food, const Snake& snake, const Monster& monster);

// Check if food is on snake's body
bool foodOnSnake(const Square& food, const Snake& snake);

// Check if food is on bomb
bool foodOnBomb(const Square& food, const std::vector<Square> bombs);

// Check if bomb is on other item
bool checkBomb(const Square& bomb, const Square& food, const Snake& snake);

// Display exit message
void displayExitMessage(bool& exit, DEMAND_TYPE demand);

// Output score
void outputScore(int score);

// Convert level
std::string convertLevel(int level);

// Read score
void readScore();

#endif