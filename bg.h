#pragma once

#include <SDL2/SDL_ttf.h>

#include <glm/glm.hpp>

#include "mesh.h"

class Bg : public Mesh {
	private:
		SDL_Rect rect = {
			0,
			0,
			16,
			32
		};

		GLuint _tex;

	public:
		Bg(bool active, glm::vec2 loc);

		void draw() override;
};
