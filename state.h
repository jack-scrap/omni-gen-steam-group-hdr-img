#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "math.h"

namespace state {
	extern unsigned int
		line,
		ln;

	static const unsigned int fps = 15;

	static const unsigned int tabWd = 2;

	static const std::string format = "%Y-%m-%d %H:%M";
}
