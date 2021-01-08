#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "prog.h"
#include "util.h"

class Mesh {
	private:
		std::vector<GLfloat> _st = util::mesh::plane(glm::vec2(1, 1));

	public:
		enum id {
			VAO,
			VBO,
			STBO
		};

		enum attr {
			POS,
			ST
		};

		enum uni {
			LOC,
			COL
		};

		GLuint _id[3];

		GLint
			_attr[2],
			_uni[2];

		Prog _prog;

		Mesh(std::vector<GLfloat> pos, std::string vtx, std::string frag, glm::vec2 loc = glm::vec2(0.0), glm::vec3 col = glm::vec3(255));

		virtual void draw() = 0;
};
