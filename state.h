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

	extern unsigned int tabWd;

	extern unsigned int speed;

	static const std::string format = "%Y-%m-%d %H:%M";
}
