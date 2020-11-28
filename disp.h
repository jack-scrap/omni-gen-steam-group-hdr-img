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

		glm::vec2 _res;

		Disp(const char* title, glm::vec2 res);

		void clear(glm::vec3 bg);

		void update();

		~Disp();
};
