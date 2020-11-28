#pragma once

#include <SDL2/SDL_ttf.h>

#include <glm/glm.hpp>

#include "mesh.h"

class Char : public Mesh {
	private:
		SDL_Rect rect = {
			0,
			0,
			16,
			32
		};

		GLuint _tex;

	public:
		Char(char c, bool active, TTF_Font* font, glm::vec2 loc);

		void draw() override;
};
