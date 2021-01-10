#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "obj.h"

namespace util {
	namespace fs {
		template <typename T>
		T rd(std::string name);

		template <>
		std::string rd(std::string name);

		template <>
		std::vector<std::string> rd(std::string name);
	}

	namespace str {
		std::vector<std::string> split(std::string buff, char delim);
		std::string join(std::vector<std::string> buff);
	}

	namespace mesh {
		std::vector<GLfloat> plane(glm::vec2 sz);

		std::vector<GLushort> strip(std::vector<GLushort> idc);

		namespace rd {
			std::vector<GLfloat> vtc(std::string name);
			std::vector<GLushort> idc(std::string name);
		}
	}

	namespace phys {
		bool aabb(Obj* p, unsigned int axis, float floor, glm::vec3 d);
	}
}
