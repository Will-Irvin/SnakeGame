#include <iostream>
#include <SDL2/SDL.h>
#include <time.h>

#include "SnakeGame.hh"

#define INIT_SCREEN_DIMENSION (800)
#define INIT_TIME_DELAY (400)

// Initialize SDL and its data structures
bool init(SDL_Window**, SDL_Renderer**);

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

	SnakeGame snakeGame = SnakeGame(renderer);

	srand(SDL_GetTicks());

	snakeGame.init(10, 10, INIT_SCREEN_DIMENSION, INIT_SCREEN_DIMENSION);

	bool quit = false;
	SDL_Event e;
	Uint64 timeDelay = INIT_TIME_DELAY;
	while (!quit) {
		Uint64 startTicks = SDL_GetTicks64();
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			snakeGame.handleEvent(e);
		}
		snakeGame.move();
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(renderer);

		snakeGame.render();

		SDL_RenderPresent(renderer);
		Uint64 finishTicks = SDL_GetTicks64() - startTicks;
		if (snakeGame.isPlaying() && finishTicks < timeDelay) {
			SDL_Delay(finishTicks);
		}
	}
	closeSDL(window, renderer);
	window = NULL;
	renderer = NULL;
	return 0;
}
