#pragma once

#include <string>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace util {
	std::string rd(std::string name);

	namespace mesh {
		std::vector<GLfloat> plane(glm::vec2 sz);
	}
}
