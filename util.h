#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

		void bound(GLfloat rng[3][2], GLfloat* vtc, unsigned int noVtc);
		void bound(Obj** obj, unsigned int noObj, GLfloat rng[3][2], glm::mat4 prev);

		namespace quad {
			std::vector<GLfloat> pos(glm::vec2 sz);
			std::vector<GLushort> idc();
		}

		namespace rd {
			std::vector<GLfloat> vtc(std::string name);
			std::vector<GLushort> idc(std::string name);
		}
	}

	namespace matr {
		glm::mat4 rot(glm::mat4 model, glm::vec3 rot);
	}

	namespace phys {
		bool aabb(Obj* p, Obj* q);
	}
}
