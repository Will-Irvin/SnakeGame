#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <string>

#include "SnakeGame.hh"
#include "TextDisplay.hh"

#define FONT_SIZE (25)
#define INIT_ACCELERATION (0)
#define INIT_APPLES (1)
#define INIT_GRID_DIMENSION (10)
#define INIT_SCREEN_DIMENSION (800)
#define INIT_TIME_DELAY (150)
#define INSTRUCTION_LINES (4)
#define MAX_HEIGHT (100)
#define MAX_WIDTH (100)
#define TICKS_FOR_60_FPS (1000 / 60)

enum Data {
	TIME_DELAY,
	ACCELERATION,
	G_WIDTH,
	G_HEIGHT,
	NUM_APPLES,
	HIGH_SCORE,
	TOTAL_DATA
};

enum GameOver {
	NEW_HIGH,
	LAST_SCORE,
	GAME_OVER,
	EXIT,
	TOTAL_GAME_OVER
};

// String constants
const std::string DATA_TEXT[] = {"Milliseconds between movement: ", 
											 "Acceleration of the snake (in ms / apple acquired): ",
											 "Width of the grid: ",
											 "Height of the grid: ",
											 "Number of apples present at one time: ",
											 "High Score: "};
const std::string GAME_OVER_TEXT[] = {"New High Score!", "Previous score: ",
																			"Game Over!", 
																			"Press \"return\" to go back to the original menu"};

// Reused SDL_Colors
const SDL_Color BLACK = {0, 0, 0, 0xff};
const SDL_Color RED = {0xff, 0, 0, 0xff};

// Initialize SDL and its data structures
bool init(SDL_Window**, SDL_Renderer**, TTF_Font**);

// Initialize the TextDisplay objects for the different attributes
bool initializeText(TextDisplay*, TextDisplay*, TextDisplay*,
										Uint64*, TTF_Font*, SDL_Renderer*, SDL_Window*);

// Set up game over data before it is rendered to the screen
bool initializeGameOver(TextDisplay*, TextDisplay*, Uint64, Uint64*);

// Helper methods to load game attribute text for editing/finished editing
inline void startEditText(TextDisplay*, Uint64*, int);
inline void endEditText(TextDisplay*, Uint64*, int);

// Helper methods to check if it is ok to increment/decrement game attributes
bool checkDecrementAttribute(Uint64*, int, SDL_Window*);
bool checkIncrementAttribute(Uint64*, int, SDL_Window*);

// Render the menu displayed before a game starts
void renderInitialization(TextDisplay*, TextDisplay*, int);

// Render the game over screen
void renderGameOver(TextDisplay*, TextDisplay*, int, int, bool);

// Free memory associated with the game, quit SDL systems
void closeSDL(SDL_Window*, SDL_Renderer*, TTF_Font*, TextDisplay*, TextDisplay*,
							TextDisplay*);

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

	*window_ptr = SDL_CreateWindow("Customized Snake Game", SDL_WINDOWPOS_UNDEFINED,
																 SDL_WINDOWPOS_UNDEFINED, INIT_SCREEN_DIMENSION,
																 INIT_SCREEN_DIMENSION, SDL_WINDOW_SHOWN);
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
 * @param window Window being rendered to, some attributes will be initialized
 * @return Whether all of the text was successfully loaded or not
 */
bool initializeText(TextDisplay* instructions_ptr, TextDisplay* dataText,
										TextDisplay* gameOverText, Uint64* gameData,
										TTF_Font* font, SDL_Renderer* renderer, SDL_Window* window) {
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

	for (int i = 0; i < TOTAL_GAME_OVER; i++) {
		gameOverText[i] = TextDisplay(font, renderer);
	}

	// Load Instructions
	bool success = true;
	std::stringstream currStr;
	currStr << "Adjust each of the values below to your liking before "
					<< "starting the game";
	success = success && instructions_ptr->loadText(currStr.str(), BLACK);

	instructions_ptr++;

	currStr.str("Use the up and down arrow keys to select which attribute to "
							"adjust");
	success = success && instructions_ptr->loadText(currStr.str(), BLACK);

	instructions_ptr++;

	currStr.str("Use the left and right arrow keys to adjust the selected value");
	success = success && instructions_ptr->loadText(currStr.str(), BLACK);

	instructions_ptr++;

	currStr.str("Press return/enter to begin playing. Use the arrow keys or "
							"\"wasd\" to move");
	success = success && instructions_ptr->loadText(currStr.str(), BLACK);

	// Initial highlighted/selected text
	currStr.str("");
	currStr << DATA_TEXT[TIME_DELAY] << gameData[TIME_DELAY];
	success = success && dataText[TIME_DELAY].loadText(currStr.str(), RED);

	// Rest of the text
	for (int i = 1; i < TOTAL_DATA; i++) {
		currStr.str("");
		currStr << DATA_TEXT[i] << gameData[i];
		success = success && dataText[i].loadText(currStr.str(), BLACK);
	}

	// Game over screen
	success = success && gameOverText[NEW_HIGH].loadText(GAME_OVER_TEXT[NEW_HIGH], BLACK);
	success = success && gameOverText[GAME_OVER].loadText(GAME_OVER_TEXT[GAME_OVER], BLACK);
	success = success && gameOverText[EXIT].loadText(GAME_OVER_TEXT[EXIT], BLACK);

	return success;
}

/**
 * Initialize any game over screen text that relies on data from the previous
 * round (last score, high score)
 * @param gameOverText An array of text objects used to display to this screen
 * @param highScoreText Pointer to the object used to display the high score
 * @param recentScore The score from the previously completed round
 * @param gameData an array of data related to the game's attributes
 * @return Whether a new high score was achieved or not
 */
bool initializeGameOver(TextDisplay* gameOverText, TextDisplay* highScoreText,
												Uint64 recentScore, Uint64* gameData) {
	std::stringstream gameOverStr;
	gameOverStr << GAME_OVER_TEXT[LAST_SCORE] << recentScore;
	gameOverText[LAST_SCORE].loadText(gameOverStr.str(), BLACK);
	if (recentScore > gameData[HIGH_SCORE]) { // Update high score
		gameData[HIGH_SCORE] = recentScore;
		gameOverStr.str("");
		gameOverStr << DATA_TEXT[HIGH_SCORE] << gameData[HIGH_SCORE];
		highScoreText->loadText(gameOverStr.str(), BLACK);
		return true;
	} else {
		return false;
	}
}

/**
 * Prepare a game attribute to be ready to be edited by loading its text
 * with the color red
 * @param dataText array of game attribute text
 * @param gameData array of game attribute values
 * @param index The attribute that will be loaded in the arrays
 */
inline void startEditText(TextDisplay* dataText, Uint64* gameData, int index) {
	std::stringstream text;
	text << DATA_TEXT[index] << gameData[index];
	dataText[index].loadText(text.str(), RED);
}

/**
 * End a game attribute's editing phase by loading its text with the color
 * black
 * @param dataText array of game attribute text
 * @param gameData array of game attribute values
 * @param index The attribute that will be loaded in the arrays
 */
inline void endEditText(TextDisplay* dataText, Uint64* gameData, int index) {
	std::stringstream text;
	text << DATA_TEXT[index] << gameData[index];
	dataText[index].loadText(text.str(), BLACK);
}

/**
 * Check whether it is safe to decrement the given attribute based on different
 * conditions
 * @param gameData array of game attributes
 * @param index The attibute being checked
 * @param window Window being rendered to, used in certain checks
 * @return Whether the value was changed or not
 */
bool checkDecrementAttribute(Uint64* gameData, int index, SDL_Window* window) {
	switch (index) {
		case TIME_DELAY:
			if (gameData[index] > 1 && gameData[index] > gameData[ACCELERATION]) {
				gameData[index]--;
				return true;
			}
			return false;
		case NUM_APPLES:
			if (gameData[index] > 1) {
				gameData[index]--;
				return true;
			}
			return false;
			break;
		case ACCELERATION:
			if (gameData[index] > 0) {
				gameData[index]--;
				return true;
			}
			return false;
			break;
		case G_WIDTH:
			gameData[index]--;
			if (gameData[index] * gameData[G_HEIGHT] < gameData[NUM_APPLES]) {
				gameData[index]++;
				return false;
			}
			return true;
			break;
		case G_HEIGHT:
			gameData[index]--;
			if (gameData[index] * gameData[G_WIDTH] < gameData[NUM_APPLES]) {
				gameData[index]++;
				return false;
			}
			return true;
			break;
	}
	return false;
}

/**
 * Check whether it is safe to increment the given attribute based on different
 * conditions
 * @param gameData array of game attributes
 * @param index The attribute being checked
 * @param window Window beign rendered to, used in certain checks
 * @return Whether the value was changed or not
 */
bool checkIncrementAttribute(Uint64* gameData, int index, SDL_Window* window) {
	switch (index) {
		case TIME_DELAY:
			if (gameData[index] < std::numeric_limits<Uint64>::max()) {
				gameData[index]++;
				return true;
			}
			return false;
			break;
		case ACCELERATION:
			if (gameData[index] < gameData[TIME_DELAY]) {
				gameData[index]++;
				return true;
			}
			return false;
			break;
		case G_WIDTH:
			gameData[index]++;
			if (gameData[index] > MAX_WIDTH) {
				gameData[index]--;
				return false;
			}
			return true;
			break;
		case G_HEIGHT:
			gameData[index]++;
			if (gameData[index] > MAX_HEIGHT) {
				gameData[index]--;
				return false;
			}
			return true;
			break;
		case NUM_APPLES:
			gameData[index]++;
			if (gameData[index] > gameData[G_WIDTH] * gameData[G_HEIGHT] - 1) {
				gameData[index]--;
				return false;
			}
			return true;
	}
	return false;		
}
			
/**
 * Render the screen for setting up the game (allowing user to change
 * attributes before playing)
 * @param instructions_ptr Pointer to TextDisplay object that displays
 													 instructions to the user on how to set up the game
 * @param windowWidth width of the window used to center text
 */
void renderInitialization(TextDisplay* instructions, TextDisplay* dataText,
													int windowWidth) {
	int y = 0;
	for (int i = 0; i < INSTRUCTION_LINES; i++) { // Instructions
		instructions[i].render((windowWidth - instructions[i].getWidth()) / 2, y);
		y += instructions[i].getHeight();
	}

	// High Score
	dataText[HIGH_SCORE].render((windowWidth - dataText[HIGH_SCORE].getWidth()) / 2, y);
	y += dataText[HIGH_SCORE].getHeight();

	// Rest of the data
	for (int i = 0; i < HIGH_SCORE; i++) {
		dataText[i].render((windowWidth - dataText[i].getWidth()) / 2, y);
		y += dataText[i].getHeight();
	}
}

/**
 * Render the game over screen to the window
 * @param gameOverText array of text specifically for the game over screen
 * @param highScoreText Pointer to object displaying the high score text
 * @param windowWidth Width of the window being rendered to
 * @param windowHieght Heigth of the window being rendered to
 * @param newHigh whether a new high score was achieved or not
 */
void renderGameOver(TextDisplay* gameOverText, TextDisplay* highScoreText,
										int windowWidth, int windowHeight, bool newHigh) {
	int y = 0;

	gameOverText[GAME_OVER].render((windowWidth -
																 gameOverText[GAME_OVER].getWidth()) / 2, y);
	y += gameOverText[GAME_OVER].getHeight();
	if (newHigh) {
		gameOverText[NEW_HIGH].render((windowWidth -
																	 gameOverText[NEW_HIGH].getWidth()) / 2, y);
		y += gameOverText[NEW_HIGH].getHeight();
	}
	highScoreText->render((windowWidth - highScoreText->getWidth()) / 2, y);
	y += highScoreText->getHeight();

	gameOverText[LAST_SCORE].render((windowWidth -
																		gameOverText[LAST_SCORE].getWidth()) / 2,
																		y);
	gameOverText[EXIT].render((windowWidth - gameOverText[EXIT].getWidth()) / 2,
														windowHeight - gameOverText[EXIT].getHeight());
}

/**
 * Free any existing memory and quit SDL systems
 * @param window SDL_Window to be destroyed
 * @param renderer SDL_Renderer to be destoryed
 * @param font Font to be destoryed
 * @param instructions Array of TextDisplay objects to be freed
 * @param dataText Array of TextDisplay objects to be freed
 * @param gameOverText Array of TextDisplay objects to be freed
 */
void closeSDL(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font,
							TextDisplay* instructions, TextDisplay* dataText,
							TextDisplay* gameOverText) {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(font);

	for (int i = 0; i < INSTRUCTION_LINES; i++) {
		instructions[i].free();
	}
	for (int i = 0; i < TOTAL_DATA; i++) {
		dataText[i].free();
	}
	for (int i = 0; i < TOTAL_GAME_OVER; i++) {
		gameOverText[i].free();
	}
	SDL_Quit();
	TTF_Quit();
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
	TextDisplay gameOverDisplay[TOTAL_GAME_OVER];
	Uint64 gameData[TOTAL_DATA];

	if (!initializeText(instructions, dataDisplay, gameOverDisplay, gameData,
											font, renderer, window)) {
		return -1;
	}

	srand(SDL_GetTicks());

	bool quit = false;
	SDL_Event e;

	// Variables for the snake game and its different attributes
	SnakeGame snakeGame = SnakeGame(renderer);

	// Current piece of data being altered
	int currIndex = 0;

	int windowWidth = INIT_SCREEN_DIMENSION;
	int windowHeight = INIT_SCREEN_DIMENSION;

	// Keep track of whether the game just finished or in initialization
	bool gameOver = false;
	// Note whether a new high score was achieved or not in the last round
	bool newHigh = false;
	while (!quit) {
		int startTicks = SDL_GetTicks64();
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_RETURN) {
					if (!snakeGame.isPlaying() && !gameOver) { // Start game
						SDL_SetWindowResizable(window, SDL_FALSE);
						snakeGame.init(gameData[G_HEIGHT], gameData[G_WIDTH], windowWidth,
													 windowHeight, gameData[NUM_APPLES]);
					} else if (gameOver) { // Exit game over screen
						gameOver = false;
					}
				} else if (e.key.keysym.sym == SDLK_UP) { // Move currIndex up
					if (!snakeGame.isPlaying() && !gameOver) {
						endEditText(dataDisplay, gameData, currIndex);
						currIndex--;
						if (currIndex < 0) {
							currIndex = NUM_APPLES;
						}
						startEditText(dataDisplay, gameData, currIndex);
					}
				} else if (e.key.keysym.sym == SDLK_DOWN) { // Move currIndex down
					if (!snakeGame.isPlaying() && !gameOver) {
						endEditText(dataDisplay, gameData, currIndex);
						currIndex++;
						if (currIndex > NUM_APPLES) {
							currIndex = 0;
						}
						startEditText(dataDisplay, gameData, currIndex);
					}
				} else if (e.key.keysym.sym == SDLK_LEFT && !snakeGame.isPlaying()
									 && !gameOver) {
					if (checkDecrementAttribute(gameData, currIndex, window)) {
						startEditText(dataDisplay, gameData, currIndex);
					}
				} else if (e.key.keysym.sym == SDLK_RIGHT && !snakeGame.isPlaying()
									 && !gameOver) {
					if (checkIncrementAttribute(gameData, currIndex, window)) {
						startEditText(dataDisplay, gameData, currIndex);
					}
				}			
			} else if (e.type == SDL_WINDOWEVENT && 
								 (e.window.event == SDL_WINDOWEVENT_RESIZED)) {
				windowWidth = e.window.data1;
				windowHeight = e.window.data2;
			}
			snakeGame.handleEvent(e);
		}
		if (!snakeGame.move()) { // Game over
			// Prepare game over screen
			newHigh = initializeGameOver(gameOverDisplay, &dataDisplay[HIGH_SCORE],
																	 snakeGame.getScore(), gameData);
			// Reset game
			snakeGame.reset();
			SDL_SetWindowResizable(window, SDL_TRUE);
			gameOver = true; // Display game over screen
		}

		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(renderer);

		if (!snakeGame.isPlaying() && !gameOver) { // Initialization
			renderInitialization(instructions, dataDisplay, windowWidth);
		} else if (gameOver) { // Game over screen
			renderGameOver(gameOverDisplay, &dataDisplay[HIGH_SCORE], windowWidth,
										 windowHeight, newHigh);
		}
		snakeGame.render();

		SDL_RenderPresent(renderer);
		int finishTime = SDL_GetTicks64() - startTicks;
		if (finishTime < 0) continue;

		int sleepTime;
		if (snakeGame.isPlaying()) {
			sleepTime = gameData[TIME_DELAY] -
									gameData[ACCELERATION] * (snakeGame.getScore() - 1) -
									finishTime;
		} else {
			sleepTime = TICKS_FOR_60_FPS - finishTime;
		}
		if (sleepTime > 0) {
			SDL_Delay(sleepTime);
		}
	}
	closeSDL(window, renderer, font, instructions, dataDisplay, gameOverDisplay);
	snakeGame.reset();
	window = NULL;
	renderer = NULL;
	font = NULL;
	return 0;
}
