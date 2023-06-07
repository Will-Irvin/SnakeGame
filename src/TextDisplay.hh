#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextDisplay {
	public:
		TextDisplay(TTF_Font*, SDL_Renderer*);
		bool loadText(std::string, SDL_Color);
		void render();

	private:
		// SDL data structures used to render text
		TTF_Font* _font;
		SDL_Renderer* _renderer;

		// Dimensions of text data
		int _width;
		int _height;

		// Texture used to display text
		SDL_Texture* textTexture;
};
