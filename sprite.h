#ifndef _SPRITE_H_
#define _SPRITE_H_

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#elif _WIN32
#include <SDL.h>
#include <SDL_image.h>
#endif

#include <string>
#include <vector>

class Sprite {
	public:
		Sprite();
		Sprite(int x, int y, int w, int h, int scale, std::string file, SDL_Renderer* renderer); //coords, size of sprite, multiply sprite size, sprite file
		~Sprite();

		void draw(SDL_Renderer* render, int x, int y);
		void draw(SDL_Renderer* render, int x, int y, SDL_RendererFlip flip);
		int getWidth();
		int getHeight();
		int getScale();
		int getX();
		int getY();
	private:
		SDL_Texture* texture;
		int xLoc;
		int yLoc;
		int spriteWidth;
		int spriteHeight;
		int entWidth;
		int entHeight;
		int scale;
		int screenX;
		int screenY;
};




#endif