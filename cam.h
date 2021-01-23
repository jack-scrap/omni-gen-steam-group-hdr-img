#pragma once

#include <glm/glm.hpp>

struct Cam {
	glm::vec3
		_pos,
		_scale,

		_prev;

	bool _drag = false;

	int
		_begin[2],
		_curr[2],
		_delta[2];
};
