#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "obj.h"

class Disp {
	private:
		SDL_GLContext _ctx;

		SDL_Window* _win;

	public:
		SDL_Renderer* _rend;

		bool _open = false;

		unsigned int _res[2];

		unsigned int _t;

		Disp(const char* title, glm::vec2 res, glm::vec3 bg);

		void clear();

		void update();

		~Disp();
};
