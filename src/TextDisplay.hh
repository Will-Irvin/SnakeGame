#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextDisplay {
	public:
		TextDisplay();
		TextDisplay(TTF_Font*, SDL_Renderer*);
		bool loadText(std::string, SDL_Color);
		void render(int, int);
		void free();
		void setFont(TTF_Font*);
		void setRenderer(SDL_Renderer*);
		int getHeight();
		int getWidth();

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
