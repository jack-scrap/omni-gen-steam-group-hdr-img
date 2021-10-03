#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "math.h"

namespace state {
	extern unsigned int lineCnt;
	extern unsigned int lineWd;

	extern unsigned int baseNo;

	extern unsigned int hlLineNo;
	extern unsigned int hlActiveLine;

	extern unsigned int tabWd;
	extern bool expandTab;

	extern unsigned int fps;

	extern float speed;

	extern std::string startDir;

	extern unsigned int showFilePerm;
	extern unsigned int showDiff;

	extern std::string format;
}
