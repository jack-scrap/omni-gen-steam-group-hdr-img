#pragma once

#include <SDL2/SDL_ttf.h>

#include <glm/glm.hpp>

#include "mesh.h"
#include "state.h"

enum mode {
	EDITOR,
	CMD
};

class Console : public Mesh {
	private:
		TTF_Font* font;

		std::vector<std::vector<bool>> _hl;

		SDL_Surface
			* canv = SDL_CreateRGBSurface(0, state::res[0], state::res[1], 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000),
			* bg = SDL_CreateRGBSurface(0, state::sz[0], state::sz[1], 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

		SDL_Rect
			canvRect = {
				0,
				0,
				state::ln * state::sz[0],
				state::view[1]
			},

			bgRect = {
				0,
				0,
				state::sz[0],
				state::sz[1]
			};

		std::vector<std::vector<SDL_Surface*>> _map;

		GLuint _tex;

	public:
		unsigned int _mode;

		std::vector<std::string> _buff;

		void render();

		void push(char c);

		void newline();

		void pop();

		Console(std::vector<std::string> buff);

		void draw() override;
};
