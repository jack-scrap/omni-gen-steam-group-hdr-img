#include <cstdlib>

#include "mesh.h"

Mesh* meshMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim) {
	Mesh* _ = (Mesh*) malloc(sizeof (Mesh));

	_->_noPrim = noPrim;

	glGenVertexArrays(1, &_->_id[Mesh::VAO]);
	glBindVertexArray(_->_id[Mesh::VAO]);

	glGenBuffers(1, &_->_id[Mesh::VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[Mesh::VBO]);
	glBufferData(GL_ARRAY_BUFFER, noPrim * 3 * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[Mesh::IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[Mesh::IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _->_noPrim * sizeof (GLushort), idc, GL_STATIC_DRAW);

	return _;
}

void meshDel(Mesh* mesh) {
	for (int i = 1; i < 4; i++) {
		if (mesh->_id[i]) {
			glDeleteBuffers(1, &mesh->_id[i]);
		}
	}

	glDeleteBuffers(1, &mesh->_id[0]);

	free(mesh);
}
