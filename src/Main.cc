#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <string>

#include "SnakeGame.hh"
#include "TextDisplay.hh"

#define FONT_SIZE (20)
#define INIT_ACCELERATION (0)
#define INIT_APPLES (1)
#define INIT_GRID_DIMENSION (10)
#define INIT_SCREEN_DIMENSION (800)
#define INIT_TIME_DELAY (150)

enum Data {
	TIME_DELAY,
	ACCELERATION,
	G_WIDTH,
	G_HEIGHT,
	NUM_APPLES,
	TOTAL_DATA
};

// Initialize SDL and its data structures
bool init(SDL_Window**, SDL_Renderer**);

// Load font, images, and text to display in between games
bool loadMedia();

// Free memory associated with the game, quit SDL systems
void closeSDL(SDL_Window*, SDL_Renderer*);

bool init(SDL_Window** window_ptr, SDL_Renderer** renderer_ptr) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Unable to initialize SDL: " << SDL_GetError() << '\n';
		return false;
	}

	*window_ptr = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
														 INIT_SCREEN_DIMENSION, INIT_SCREEN_DIMENSION, SDL_WINDOW_SHOWN);
	if (*window_ptr == NULL) {
		std::cout << "Unable to create window: " << SDL_GetError() << '\n';
		return false;
	}

	*renderer_ptr = SDL_CreateRenderer(*window_ptr, -1, SDL_RENDERER_ACCELERATED);
	if (*renderer_ptr == NULL) {
		std::cout << "Unable to create renderer: " << SDL_GetError() << '\n';
		return false;
	}
	return true;
}

void closeSDL(SDL_Window* window, SDL_Renderer* renderer) {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_Quit();
}
		
int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	if (!init(&window, &renderer)) {
		return false;
	}


	srand(SDL_GetTicks());


	bool quit = false;
	SDL_Event e;

	// Variables for the snake game and its different attributes
	SnakeGame snakeGame = SnakeGame(renderer);
	Uint64 gameData[TOTAL_DATA];
	gameData[TIME_DELAY] = INIT_TIME_DELAY;
	gameData[ACCELERATION] = INIT_ACCELERATION;
	gameData[G_WIDTH] = INIT_GRID_DIMENSION;
	gameData[G_HEIGHT] = INIT_GRID_DIMENSION;
	gameData[NUM_APPLES] = INIT_APPLES;

	// Current piece of data being altered
	//int currIndex = 0;

	int windowWidth = INIT_SCREEN_DIMENSION;
	int windowHeight = INIT_SCREEN_DIMENSION;

	// Keep track of whether the game just finished or in initialization
	//bool gameOver = false;
	while (!quit) {
		int startTicks = SDL_GetTicks64();
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_RETURN && !snakeGame.isPlaying()) {
					// Get window dimensions before playing
					SDL_GetWindowSize(window, &windowWidth, &windowHeight);
					SDL_SetWindowResizable(window, SDL_FALSE);
					snakeGame.init(gameData[G_WIDTH], gameData[G_HEIGHT], windowWidth, windowHeight);
				}
			}
			snakeGame.handleEvent(e);
		}
		if (!snakeGame.move()) {
			snakeGame.reset();
		}
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(renderer);

		snakeGame.render();

		SDL_RenderPresent(renderer);
		int finishTime = SDL_GetTicks64() - startTicks;
		if (finishTime < 0) continue;

		int sleepTime = gameData[TIME_DELAY] - finishTime;
		if (snakeGame.isPlaying() && sleepTime > 0) {
			SDL_Delay(sleepTime);
		}
	}
	closeSDL(window, renderer);
	snakeGame.reset();
	window = NULL;
	renderer = NULL;
	return 0;
}
