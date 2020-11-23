#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>

class Disp {
	private:
		SDL_Window* win;

		SDL_GLContext ctx;

		bool open = false;

	public:
		int
			_wd,
			_ht;

		Disp(const char* title, int wd, int ht);

		void clear(float r, float g, float b);

		void update();

		~Disp();
};
