#pragma once

#include <SDL2/SDL_ttf.h>

#include <glm/glm.hpp>

#include "mesh.h"

class Console : public Mesh {
	private:
		std::vector<std::vector<bool>> _active;

		SDL_Rect
			bgRect = {
				0,
				0,
				800 / 2,
				600
			},

			charRect = {
				0,
				0,
				16,
				32
			};

		GLuint _tex;

	public:
		Console(std::vector<std::string> buff);

		void draw() override;
};
