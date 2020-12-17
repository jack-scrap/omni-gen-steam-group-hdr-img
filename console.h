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
			* _canv = SDL_CreateRGBSurface(0, state::res[0], state::res[1], 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000),
			* _bg = SDL_CreateRGBSurface(0, state::dim[0], state::dim[1], 4 * sizeof (long int), 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

		SDL_Rect
			_canvRect = {
				0,
				0,
				state::ln * state::dim[0],
				state::view[1]
			},

			_bgRect = {
				0,
				0,
				state::dim[0],
				state::dim[1]
			};

		std::vector<std::vector<SDL_Surface*>> _map;

		GLuint _tex;

	public:
		unsigned int
			_mode,

			_idx[2];

		std::vector<std::string> _buff;

		std::string _cmd;

		void render();

		void push(char c);

		void enter();

		void newline();

		void exec();

		void pop();

		Console(std::vector<std::string> buff);

		void draw() override;
};
