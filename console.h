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

		std::vector<std::vector<bool>> _active;

		SDL_Rect
			bgRect = {
				0,
				0,
				state::ln * state::sz[0],
				600
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
