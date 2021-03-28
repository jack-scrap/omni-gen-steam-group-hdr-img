#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "math.h"

namespace state {
	extern unsigned int
		line,
		ln;

	extern unsigned int fps;

	static const unsigned int tabWd = 2;

	static const std::string format = "%Y-%m-%d %H:%M";
}
