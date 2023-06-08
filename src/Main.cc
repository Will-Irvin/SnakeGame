#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <string>

#include "SnakeGame.hh"
#include "TextDisplay.hh"

#define FONT_SIZE (25)
#define FORMAT_PIXELS (20)
#define INIT_ACCELERATION (0)
#define INIT_APPLES (1)
#define INIT_GRID_DIMENSION (10)
#define INIT_SCREEN_DIMENSION (800)
#define INIT_TIME_DELAY (150)
#define INSTRUCTION_LINES (5)

enum Data {
	TIME_DELAY,
	ACCELERATION,
	G_WIDTH,
	G_HEIGHT,
	NUM_APPLES,
	HIGH_SCORE,
	TOTAL_DATA
};

// Initialize SDL and its data structures
bool init(SDL_Window**, SDL_Renderer**, TTF_Font**);

// Initialize the TextDisplay objects for the different attributes
bool initializeText();

// Render the menu displayed before a game starts
void renderInitialization(TextDisplay*, int);

// Free memory associated with the game, quit SDL systems
void closeSDL(SDL_Window*, SDL_Renderer*);

/**
 * Initialize SDL and TTF functions, load window, renderer, and font into
 * the given pointers
 * @param window_ptr Pointer for the SDL_Window that will be used during program
 * @param renderer_ptr Pointer for the renderer that will be used during program
 * @param font_ptr Pointer for the font that will be used during program
 * @return Whether all of the initialization successfully completed
 */
bool init(SDL_Window** window_ptr, SDL_Renderer** renderer_ptr, TTF_Font** font_ptr) {
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

	if (TTF_Init() == -1) {
		std::cout << "TTF Initialization Error: " << TTF_GetError() << '\n';
		return false;
	}

	*font_ptr = TTF_OpenFont("fonts/BebasNeue-Regular.ttf", FONT_SIZE);
	if (*font_ptr == NULL) {
		std::cout << "Unable to load font: " << TTF_GetError() << '\n';
		return false;
	}
	return true;
}

/**
 * Load initial data and text into their respective places
 * Use the renderer and font that was initialized previously when making
 * these new TextDisplay objects
 * @param instruction_ptr The object that will display instructions to the player
 * @param dataText An array of TextDisplay objects that will display each of
 									 the attributes that can be altered by the user
 * @param gameData An array containing the data that will be displayed/altered
 * @param font The font that will be used when displaying text
 * @param renderer The renderer used to render each of the text images
 * @return Whether the text was successfully loaded or not
 */
bool initializeText(TextDisplay* instructions_ptr, TextDisplay* dataText,
										Uint64* gameData, TTF_Font* font, SDL_Renderer* renderer) {
	// Initialize data array
	gameData[TIME_DELAY] = INIT_TIME_DELAY;
	gameData[ACCELERATION] = INIT_ACCELERATION;
	gameData[G_WIDTH] = INIT_GRID_DIMENSION;
	gameData[G_HEIGHT] = INIT_GRID_DIMENSION;
	gameData[NUM_APPLES] = INIT_APPLES;
	gameData[HIGH_SCORE] = 0;

	for (int i = 0; i < INSTRUCTION_LINES; i++) {
		instructions_ptr[i] = TextDisplay(font, renderer);
	}

	for (int i = 0; i < TOTAL_DATA; i++) {
		dataText[i] = TextDisplay(font, renderer);
	}

	SDL_Color regularText = {0, 0, 0, 0xff}; // Black for regular text

	// Load Instructions
	bool success = true;
	std::stringstream currStr;
	currStr << "Adjust each of the values below to your liking before "
					<< "starting the game";
	success = success && instructions_ptr->loadText(currStr.str(), regularText);
	instructions_ptr++;

	currStr.str("Use the up and down arrow keys to select which attribute to "
							"adjust");
	success = success && instructions_ptr->loadText(currStr.str(), regularText);
	instructions_ptr++;

	currStr.str("Use the left and right arrow keys to adjust the selected value");
	success = success && instructions_ptr->loadText(currStr.str(), regularText);
	instructions_ptr++;

	currStr.str("");
	currStr << "Press the \"R\" key to resize the window such that the grid "
				  << "will be viewed as " << FORMAT_PIXELS << " pixel squares";
	success = success && instructions_ptr->loadText(currStr.str(), regularText);
	instructions_ptr++;

	currStr.str("Press return/enter to begin playing. Use the arrow keys or "
							"\"wasd\" to move");
	success = success && instructions_ptr->loadText(currStr.str(), regularText);
	
	return success;
}

/**
 * Render the screen for setting up the game (allowing user to change
 * attributes before playing)
 * @param instructions_ptr Pointer to TextDisplay object that displays
 													 instructions to the user on how to set up the game
 * @param windowWidth width of the window used to center text
 */
void renderInitialization(TextDisplay* instructions_ptr,
													int windowWidth) {
	int y = 0;
	for (int i = 0; i < INSTRUCTION_LINES; i++) {
		TextDisplay currInstruction = instructions_ptr[i];
		currInstruction.render((windowWidth - currInstruction.getWidth()) / 2, y);
		y += currInstruction.getHeight();
	}
}

void closeSDL(SDL_Window* window, SDL_Renderer* renderer) {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_Quit();
}
		
int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	TTF_Font* font = NULL;

	if (!init(&window, &renderer, &font)) {
		return -1;
	}
	
	TextDisplay instructions[INSTRUCTION_LINES];
	TextDisplay dataDisplay[TOTAL_DATA];
	Uint64 gameData[TOTAL_DATA];

	initializeText(instructions, dataDisplay, gameData, font, renderer);

	srand(SDL_GetTicks());

	bool quit = false;
	SDL_Event e;

	// Variables for the snake game and its different attributes
	SnakeGame snakeGame = SnakeGame(renderer);

	// Current piece of data being altered
	//int currIndex = 0;

	int windowWidth = INIT_SCREEN_DIMENSION;
	int windowHeight = INIT_SCREEN_DIMENSION;

	// Keep track of whether the game just finished or in initialization
	bool gameOver = false;
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

		if (!snakeGame.isPlaying() && !gameOver) {
			renderInitialization(instructions, windowWidth);
		}

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
