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

	public:
		bool _open = false;

		SDL_Window* _win;

		SDL_Renderer* _rend;

		SDL_Surface* _map;

		unsigned int _res[2];

		unsigned int _t;

		Disp(const char* title, glm::vec2 res, glm::vec3 bg);

		void clear();

		void update();

		void draw();

		~Disp();
};
