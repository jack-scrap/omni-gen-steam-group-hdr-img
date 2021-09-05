#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "math.h"

namespace state {
	extern unsigned int
		line,
		ln;

	extern unsigned int initIdx;

	extern unsigned int hlLineNo;
	extern unsigned int hlActiveLine;

	extern unsigned int fps;

	extern unsigned int tabWd;

	extern unsigned int speed;

	extern std::string format;
}
