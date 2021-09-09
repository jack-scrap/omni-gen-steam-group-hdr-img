#pragma once

#include <glm/glm.hpp>

struct Cam {
	glm::vec3 _pos;
	glm::vec3 _scale;

	bool _drag = false;

	int _begin[2];
	int _curr[2];
	int _delta[2];
};
