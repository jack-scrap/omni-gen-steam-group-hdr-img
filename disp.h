#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

class Disp {
	private:
		bool open = false;

	public:
		SDL_GLContext ctx;

		SDL_Window* win;

		int
			_wd,
			_ht;

		Disp(const char* title, int wd, int ht);

		void clear(glm::vec3 bg);

		void update();

		~Disp();
};
