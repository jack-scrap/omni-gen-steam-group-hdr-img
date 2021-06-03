#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "obj.h"

class Disp {
	public:
		bool _open = false;

		SDL_GLContext _ctx;

		SDL_Window* _win;

		unsigned int _res[2];

		unsigned int _t;

		Disp(const char* title, glm::vec2 res, glm::vec3 bg);

		void clear();

		void update();

		void draw();

		~Disp();
};
