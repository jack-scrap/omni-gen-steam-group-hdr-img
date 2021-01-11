#pragma once

#include <glm/glm.hpp>

struct Cam {
	glm::vec3
		_pos,
		_scale;

	bool _drag;

	glm::vec3 _prev;

	int
		_begin[2],
		_curr[2],
		_delta[2];
};
