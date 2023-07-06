#include <assert.h>
#include <cstdio>
#include <iostream>
#include <queue>
#include <SDL2/SDL.h>
#include <sstream>
#include <string>

#include "SnakeGame.hh"

#define RIGHT_ANGLE (90)

// Initialize variables
SnakeGame::SnakeGame(SDL_Renderer* renderer, SDL_Texture* texture) {
	_grid = NULL;
	_nRows = 0;
	_nCols = 0;
	_currLoc = std::pair(-1, -1);
	_direction = NONE;

	_score = 0;
	_playing = false;

	_renderer = renderer;
	_texture = texture;
}

/**
 * Setup the grid to have the given dimensions
 * Get the rest of the game ready to begin
 * @param nRows: number of rows in the grid of the new game
 * @param nCols: number of columns in the grid of the new game
 * @param numApples: number of apples initially placed on the board
 */
void SnakeGame::init(int nRows, int nCols, int numApples) {
	reset();
	// Initialize grid, set current location
	_nRows = nRows;
	_nCols = nCols;
	_grid = (Spaces *) malloc(_nRows * _nCols * sizeof(Spaces));
	_currLoc.first = _nRows / 2;
	_currLoc.second = _nCols / 2;

	// Set up grid for the start of the game
	for (int r = 0; r < _nRows; r++) {
		int offset = r * _nCols;
		for (int c = 0; c < _nCols; c++) {
			*(_grid + offset + c) = BLANK;
			_freeCells.push_back(offset + c);
		}
	}
	setCell(_currLoc.first, _currLoc.second, HEAD);
	deleteFreeSpace(_currLoc.first, _currLoc.second);

	// Set first apple
	for (int i = 0; i < numApples; i++) {
		placeApple();
	}

	_score = 1;
	_playing = true;
}

/**
 * Free any memory associated with an active SnakeGame
 * Reset variables to their initial state
 */
void SnakeGame::reset() {
	if (_grid != NULL) {
		free(_grid);
		_freeCells.clear();
		while (!_path.empty()) {
			_path.pop();
		}
		_grid = NULL;
		_nRows = 0;
		_nCols = 0;
		_currLoc = std::pair(-1, -1);
		_score = 0;
		_direction = NONE;
	}
}

/**
 * Change the direction the snake is moving based on keyboard input
 * @param e The event being processed
 */
void SnakeGame::handleEvent(SDL_Event e) {
	// Only handle events if the game is happening, and on the first key press
	if (_playing && e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		SDL_Keycode key	= e.key.keysym.sym;
		// Set direction depending on the key (wasd or arrows)
		// Do not allow to move back into itself
		if ((key == SDLK_UP || key == SDLK_w) &&
				(_path.empty() ||
				!coordsEqual(std::pair(_currLoc.first - 1, _currLoc.second), _path.back()))) {
			_direction = UP;
		} else if ((key == SDLK_DOWN || key == SDLK_s) &&
							 (_path.empty() ||
							 !coordsEqual(std::pair(_currLoc.first + 1, _currLoc.second), _path.back()))) {
			_direction = DOWN;
		} else if ((key == SDLK_LEFT || key == SDLK_a) &&
							 (_path.empty() ||
							 !coordsEqual(std::pair(_currLoc.first, _currLoc.second - 1), _path.back()))) {
			_direction = LEFT;
		} else if ((key == SDLK_RIGHT || key == SDLK_d) &&
							 (_path.empty() ||
							 !coordsEqual(std::pair(_currLoc.first, _currLoc.second + 1), _path.back()))) {
			_direction = RIGHT;
		}
	}
}

/**
 * Render the game to the window
 * Each type of block will be a different color rectangle
 * A border will be displayed to outline the grid
 */
void SnakeGame::render() {
	if (_playing && _renderer != NULL) {
		// Rectangle used to render to different portions of the screen
		SDL_Rect viewport;
		SDL_RenderGetViewport(_renderer, &viewport);
		SDL_Rect currSection = {0, 0, viewport.w / _nCols, viewport.h / _nRows};
		for (int r = 0; r < _nRows; r++) {
			int offset = r * _nCols;
			for (int c = 0; c < _nCols; c++) {
				// Draw square depending on what the space is
				switch (*(_grid + offset + c)) {
					case APPLE: // Red for apple
						SDL_SetRenderDrawColor(_renderer, 0xff, 0, 0, 0xff);
						break;
					case BODY:
					case HEAD: // Green for body / head
						SDL_SetRenderDrawColor(_renderer, 0, 0xff, 0, 0xff);
						break;
					case BLANK: // Black for other spaces
						SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0xff);
						break;
				}
				if (_texture != NULL && *(_grid + offset + c) == HEAD) {
					SDL_RenderCopyEx(_renderer, _texture, NULL, &currSection,
													 RIGHT_ANGLE * _direction, NULL, SDL_FLIP_NONE);
				} else {
					SDL_RenderFillRect(_renderer, &currSection);
				}

				// Draw gray outline
				SDL_SetRenderDrawColor(_renderer, 128, 128, 128, 0xff);
				SDL_RenderDrawRect(_renderer, &currSection);

				currSection.x += currSection.w; // Go to next column
			}
			// Go to next row
			currSection.x = 0;
			currSection.y += currSection.h;
		}			
	}
}

/**
 * Move the body of the snake based on the current direction
 * Adjust the grid, score, queue, and other feature appropriately
 * Add a new apple if the game is continuing
 * @return True if the game continues (no collision), false if the game
 					 is over (ran into body or boundaries)
 */
bool SnakeGame::move() {
	if (!_playing) {
		return true;
	}
	// Adjust position of head
	if (_direction != NONE) {
		setCell(_currLoc.first, _currLoc.second, BODY);
		_path.push(_currLoc);
	}
	switch (_direction) {
		case UP:
			_currLoc.first--;
			break;
		case DOWN:
			_currLoc.first++;
			break;
		case LEFT:
			_currLoc.second--;
			break;
		case RIGHT:
			_currLoc.second++;
			break;
		case NONE:
			return true;
	}

	if (_currLoc.first >= _nRows || _currLoc.second >= _nCols ||
			_currLoc.first < 0 || _currLoc.second < 0) { // Out of bounds
		_playing = false;
		return false;
	}
	Spaces currSpace = getCell(_currLoc.first, _currLoc.second);

	assert(currSpace != HEAD);

	if (currSpace == BODY) { // Ran into itself
		_playing = false;
		return false;
	} else if (currSpace == BLANK) { // Clear last cell
		std::pair<int, int> lastLoc = _path.front();
		_path.pop();
		setCell(lastLoc.first, lastLoc.second, BLANK);

		// Add newly free space to selection for placing an apple
		addFreeSpace(lastLoc.first, lastLoc.second);

		// Remove new space from selection for placing an apple
		deleteFreeSpace(_currLoc.first, _currLoc.second);
	} else if (currSpace == APPLE) { // Increase score/length of snake
		_score++;
		if (!placeApple()) { // Place new apple, quit if an apple can't be placed
			_playing = false;
			return false;
		}
	}

	setCell(_currLoc.first, _currLoc.second, HEAD);

	return true;
}

/**
 * Place an apple at a randomly selected blank space
 * @return true if an apple could be placed successfully and false if there is
 *				 no space to place an apple.
 */
bool SnakeGame::placeApple() {
	if (_freeCells.empty()) {
		return false;
	}

	int index = rand() % _freeCells.size();
	*(_grid + _freeCells[index]) = APPLE;
	_freeCells.erase(_freeCells.begin() + index);
	return true;
}

// Add an offset indicated by the row and column number to the freeCells array
void SnakeGame::addFreeSpace(int r, int c) {
	_freeCells.push_back(r * _nCols + c);
}

// Remove the offset indicated by the row and column number from the vector
void SnakeGame::deleteFreeSpace(int r, int c) {
	int offset = r * _nCols + c;
	for (size_t i = 0; i < _freeCells.size(); i++) {
		if (_freeCells[i] == offset) {
			_freeCells.erase(_freeCells.begin() + i);
			break;
		}
	}
}

// Check if two sets of coordinates are the same
inline bool SnakeGame::coordsEqual(std::pair<int, int> coord1,
																	 std::pair<int, int> coord2) {
	return coord1.first == coord2.first && coord1.second == coord2.second;
}

// Getters

bool SnakeGame::isPlaying() {
	return _playing;
}

Uint64 SnakeGame::getScore() {
	return _score;
}

/**
 * Print a formatted table displaying the contents of the gird
 */
void SnakeGame::print() {
	if (_playing) {
		std::stringstream result;
		result << ' ';
		for (int i = 0; i < _nCols * 2; i++) {
			result << '-';
		}
		result << '\n';
		for (int r = 0; r < _nRows; r++) {
			int offset = r * _nCols;
			result << '|';
			for (int c = 0; c < _nCols; c++) {
				switch (*(_grid + offset + c)) {
					case HEAD:
						result << "H,";
						break;
					case BODY:
						result << "B,";
						break;
					case APPLE:
						result << "A,";
						break;
					case BLANK:
						result << " ,";
						break;
				}
			}
			result << "|\n";
		}
		std::cout << result.str();
	}
}

// Helper method to access particular cell on the grid
inline Spaces SnakeGame::getCell(int r, int c) {
	assert(r < _nRows && c < _nCols);
	return *(_grid + r * _nCols + c);
}

// Helper method to set a particular cell to a new value
inline void SnakeGame::setCell(int r, int c, Spaces newVal) {
	assert(r < _nRows && c < _nCols);
	*(_grid + r * _nCols + c) = newVal;
}
