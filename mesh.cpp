#include <iostream>
#include <thread>
#include <chrono>

#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"

Mesh::Mesh(std::vector<GLfloat> pos, std::string vtx, std::string frag, glm::vec2 loc, glm::vec3 col) :
	_prog(vtx, frag),
	_pos(pos),
	_loc(loc) {
		/* data */
		glGenVertexArrays(1, &_id[VAO]);
		glBindVertexArray(_id[VAO]);

		glGenBuffers(2, &_id[1]);

		glBindBuffer(GL_ARRAY_BUFFER, _id[VBO]);
		glBufferData(GL_ARRAY_BUFFER, _pos.size() * sizeof (GLfloat), &_pos[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, _id[STBO]);
		glBufferData(GL_ARRAY_BUFFER, _st.size() * sizeof (GLfloat), &_st[0], GL_STATIC_DRAW);

		/* shader */
		_prog.use();

		// attribute
		glBindBuffer(GL_ARRAY_BUFFER, _id[VBO]);
		_attr[POS] = glGetAttribLocation(_prog.id, "pos");
		glVertexAttribPointer(_attr[POS], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attr[POS]);

		glBindBuffer(GL_ARRAY_BUFFER, _id[STBO]);
		_attr[ST] = glGetAttribLocation(_prog.id, "st");
		glVertexAttribPointer(_attr[ST], 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
		glEnableVertexAttribArray(_attr[ST]);

		// uniform
		_uni[LOC] = glGetUniformLocation(_prog.id, "loc");
		glUniform2fv(_uni[LOC], 1, glm::value_ptr(_loc));

		_uni[COL] = glGetUniformLocation(_prog.id, "col");
		glUniform3fv(_uni[COL], 1, glm::value_ptr(col));
	}

void Mesh::mv(glm::vec2 d) {
	for (int i = 0; i < _fps; i++) {
		_loc += d;

		_prog.use();

		glUniform2fv(_uni[LOC], 1, glm::value_ptr(_loc));

		_prog.unUse();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / _fps));
	}
}

std::vector<GLfloat> util::mesh::plane(glm::vec2 sz) {
	std::vector<GLfloat> _;

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			_.push_back(x * sz[0]);
			_.push_back(y * sz[1]);
		}
	}

	return _;
}
