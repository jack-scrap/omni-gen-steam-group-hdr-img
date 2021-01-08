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
		GLuint _id[3];

		GLint _attr[2];

		Prog _prog;

		Mesh(std::vector<GLfloat> pos, std::string vtx, std::string frag);

		virtual void draw() = 0;
};
