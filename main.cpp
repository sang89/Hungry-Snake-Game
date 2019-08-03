#include "Snake.h"
#include "Initialization.h"
#include "Scene.h"

int main(int argc, char* argv[])
{
	// Initialize SDL and create window
	if (!init())
		printf("Cannot create window and initialize SDL!\n");

	else
	{
		// Load media
		if (!loadMedia())
			printf("Cannot load media!\n");

		else
		{
			// Exit to window flag
			bool exit = false;
			bool end = false;

			// See these functions implementations in Scene.cpp
			while (!exit)
			{
				start_up(exit);
				if (exit) break;
			    play_game(exit, end);
				if (exit) break;
				
				if (end)
					end_game();
				
			}
		}
	}

	// Close SDL and free memories
	close();

	return 0;
}