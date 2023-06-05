#include <assert.h>
#include <cstdio>
#include <iostream>
#include <queue>
#include <SDL2/SDL.h>
#include <sstream>
#include <string>

#include "SnakeGame.hh"

// Initialize variables
SnakeGame::SnakeGame() {
	_grid = NULL;
	_nRows = 0;
	_nCols = 0;
	_currLoc = std::pair(-1, -1);
	_direction = NONE;

	_score = -1;
	_playing = false;
}

/**
 * Setup the grid to have the given dimensions
 * Get the rest of the game ready to begin
 * @param nRows: number of rows in the grid of the new game
 * @param nCols: number of columns in the grid of the new game
 */
void SnakeGame::init(int nRows, int nCols) {
	reset();
	// Initialize grid, set current location
	_nRows = nRows;
	_nCols = nCols;
	_grid = (Spaces *) malloc(_nRows * _nCols * sizeof(Spaces));
	_currLoc.first = _nRows / 2;
	_currLoc.second = _nCols / 2;

	// Set up grid for the start of the game
	for (int r = 0; r < _nRows; r++) {
		for (int c = 0; c < _nCols; c++) {
			setCell(r, c, BLANK);
		}
	}
	setCell(_currLoc.first, _currLoc.second, HEAD);
	_path.push(_currLoc);

	// Set first apple


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
		_grid = NULL;
		_nRows = 0;
		_nCols = 0;
		_currLoc = std::pair(-1, -1);
		_score = -1;
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
		return false;
	}
	// Adjust position of head
	if (_direction != NONE) {
		setCell(_currLoc.first, _currLoc.second, BODY);
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
	} else if (currSpace == APPLE) { // Increase score/length of snake
		_score++;
	}

	_path.push(_currLoc);
	setCell(_currLoc.first, _currLoc.second, HEAD);

	// Set new apple location

	return true;
}

// Getters
bool SnakeGame::isPlaying() {
	return _playing;
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

void SnakeGame::setDirection(Direction direction) {
	_direction = direction;
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
