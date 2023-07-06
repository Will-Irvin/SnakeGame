#include <queue>
#include <SDL2/SDL.h>

enum Spaces {
	HEAD,
	BODY,
	APPLE,
	BLANK
};

enum Direction {
	DOWN,
	LEFT,
	UP,
	RIGHT,
	NONE
};

class SnakeGame {
	public:
		SnakeGame(SDL_Renderer* = NULL, SDL_Texture* = NULL);
		void init(int, int, int);
		void reset();
		void handleEvent(SDL_Event);
		void render();
		bool move();
		
		// Getters
		bool isPlaying();
		Uint64 getScore();

		// Methods for testing
		void print();

	private:
		// Store what is going on at each cell of the grid (apple, blank, etc.)
		Spaces* _grid;

		// SDL renderer used to display the game
		SDL_Renderer* _renderer;

		// Texture used to display head
		SDL_Texture* _texture;

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

		Uint64 _score; // Also serves as length of snake

		bool _playing; // Whether the game has started/finished
		

		// Helper methods that should only be used while the game is being played
		bool placeApple();
		void addFreeSpace(int, int);
		void deleteFreeSpace(int, int);
		inline bool coordsEqual(std::pair<int, int>, std::pair<int, int>);

		// Helper methods to access the grid
		inline Spaces getCell(int, int);
		inline void setCell(int, int, Spaces);
};
