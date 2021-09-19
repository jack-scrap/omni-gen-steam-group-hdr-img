#pragma once

#include <glm/glm.hpp>

struct Cam {
	glm::vec3 _pos = glm::vec3(1000.0, 1000.0, 1000.0);
	glm::vec3 _scale = glm::vec3(50, 50, 50);

	glm::vec3 _posPrev = _pos;
	glm::vec3 _posDelta = glm::vec3(0.0);

	int _begin[2];
	int _curr[2];
	int _delta[2];

	bool _drag = false;
};
