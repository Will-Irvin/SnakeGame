#include <iostream>
#include <SDL2/SDL.h>
#include <time.h>

#include "SnakeGame.hh"

// Initialize SDL and its data structures
bool init();

// Free memory associated with the game, quit SDL systems
void closeSDL();

int main(int argc, char* argv[]) {
	SnakeGame snakeGame = SnakeGame();

	srand(time(0));

	snakeGame.init(10, 10);
	while (snakeGame.move()) {
		Direction direction = NONE;
		switch (rand() % 4 + 1) {
			case UP:
				direction = UP;
				break;
			case DOWN:
				direction = DOWN;
				break;
			case LEFT:
				direction = LEFT;
				break;
			case RIGHT:
				direction = RIGHT;
				break;
		}
			
		snakeGame.setDirection(direction); 
		snakeGame.print();
	}
	return 0;
}
