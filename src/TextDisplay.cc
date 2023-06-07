#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "TextDisplay.hh"

// Initialize variables to be ready to load text from the given font
TextDisplay::TextDisplay(TTF_Font* font, SDL_Renderer* renderer) {
	_width = 0;
	_height = 0;
	textTexture = NULL;

	_font = font;
	_renderer = renderer;
}

/**
 * Load given string into the texture to be renderered
 * Text will have font that the texture was initialized with
 * @param text Text that will be displayed
 * @param color Color that the text will appear in
 * @return Whether the texture was successfully created or not
 */
bool TextDisplay::loadText(std::string text, SDL_Color color) {
	if (_renderer == NULL) {
		std::cout << "No renderer was given to render text\n";
		return false;
	}
	SDL_Surface* textSurface = TTF_RenderText_Solid(_font, text.c_str(), color);
	if (textSurface == NULL) {
		std::cout << "Text Surface Creation Error: " << TTF_GetError() << '\n';
		return false;
	}

	textTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
	if (textTexture == NULL) {
		std::cout << "Texture conversion error: " << SDL_GetError() << '\n';
		return false;
	}

	_width = textSurface->w;
	_height = textSurface->h;
	SDL_FreeSurface(textSurface);
	return true;
}

/**
 * Render the text in this object at the given coordinates
 * Only render if the SDL structures are all initialized
 * @param x, y Coordinates where the text will be rendered
 */
void TextDisplay::render(int x, int y) {
	if (_renderer != NULL && textTexture != NULL) {
		SDL_Rect rect = {x, y, _width, _height};
		SDL_RenderCopy(_renderer, textTexture, NULL, &rect);
	}
}
