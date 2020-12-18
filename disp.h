#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

class Disp {
	private:
		glm::vec3 _bg;

	public:
		bool open = false;

		SDL_GLContext ctx;

		SDL_Window* win;

		glm::vec2 _res;

		Disp(const char* title, glm::vec2 res, glm::vec3 bg);

		void clear();

		void update();

		~Disp();
};
