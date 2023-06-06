#include <queue>
#include <SDL2/SDL.h>

enum Spaces {
	HEAD,
	BODY,
	APPLE,
	BLANK
};

enum Direction {
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class SnakeGame {
	public:
		SnakeGame();
		void init(int, int);
		void reset();
		void handleEvent(SDL_Event);
		bool move();
		bool placeApple();
		void addFreeSpace(int, int);
		void deleteFreeSpace(int, int);

		// Getters
		bool isPlaying();
		int getScore();

		// Methods for testing
		void print();
		void setDirection(Direction direction);

	private:
		// Store what is going on at each cell of the grid (apple, blank, etc.)
		Spaces* _grid;
		// Dimensions of the grid
		int _nRows;
		int _nCols;

		// Current location in the grid
		std::pair<int, int> _currLoc;

		// Keep track of the current path of the snake to
		// easily adjust as it continues to move
		std::queue< std::pair<int, int> > _path;

		// Keep track of the options for free cells to randomly select
		std::vector<int> _freeCells;

		// Keep track of the snakes current direction
		Direction _direction;

		int _score; // Also serves as length of snake

		bool _playing; // Whether the game has started/finished

		// Helper methods to access the grid
		inline Spaces getCell(int, int);
		inline void setCell(int, int, Spaces);
};
