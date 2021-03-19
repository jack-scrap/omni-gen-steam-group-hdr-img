#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <map>
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

		void write(std::string name, std::vector<std::string> buff);

		std::vector<std::map<std::string, std::string>> ls(std::string name);

		std::string name(std::string buff);

		std::string base(std::string buff);
	}

	namespace str {
		std::vector<std::string> split(std::string buff, char delim);
		std::string join(std::vector<std::string> buff);

		std::string pad(std::string buff, unsigned int roof);
	}

	namespace mesh {
		std::vector<GLfloat> plane(glm::vec2 sz);

		std::vector<GLushort> strip(std::vector<GLushort> idc);

		void bound(GLfloat rng[3][2], GLfloat* vtc, unsigned int noVtc);
		void bound(Obj** obj, unsigned int noObj, GLfloat rng[3][2], glm::mat4 prev);

		namespace quad {
			std::vector<GLfloat> pos(glm::vec2 sz);
			std::vector<GLfloat> pos1(glm::vec2 sz);
			std::vector<GLushort> idc();
		}

		namespace rd {
			std::vector<GLfloat> vtc(std::string name);
			std::vector<GLushort> idc(std::string name);
		}
	}

	namespace matr {
		glm::mat4 rot(glm::mat4 model, glm::vec3 rot);

		glm::vec3 apply(glm::vec3 vtx, glm::mat4 model);
	}

	namespace phys {
		bool aabb(Obj* p, Obj* q);
	}

	std::vector<std::string> log(unsigned int wd, unsigned int loc);

	std::string now(std::string format);
}
