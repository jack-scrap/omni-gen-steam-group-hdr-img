#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

inline glm::vec3 col[2] = {
	glm::vec3(38, 38, 38),
	glm::vec3(255, 155, 71)
};

inline SDL_Color colS[2] = {
	{
		38, 38, 38, 255
	}, {
		255, 155, 71, 255
	}
};
