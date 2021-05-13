#include <cstdlib>

#include "mesh.h"

Mesh* meshMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc) {
	Mesh* _ = (Mesh*) malloc(sizeof (Mesh));

	_->_noIdc = noIdc;

	glGenVertexArrays(1, &_->_id[Mesh::VAO]);
	glBindVertexArray(_->_id[Mesh::VAO]);

	glGenBuffers(1, &_->_id[Mesh::VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[Mesh::VBO]);
	glBufferData(GL_ARRAY_BUFFER, noVtc * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[Mesh::IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[Mesh::IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _->_noIdc * sizeof (GLushort), idc, GL_STATIC_DRAW);

	return _;
}
