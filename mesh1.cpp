#include <cstdlib>

#include "mesh1.h"

Mesh1* mesh1Mk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noIdc) {
	Mesh1* _ = (Mesh1*) malloc(sizeof (Mesh1));

	glGenVertexArrays(1, &_->_id[VAO]);
	glBindVertexArray(_->_id[VAO]);

	glGenBuffers(1, &_->_id[VBO]);
	glBindBuffer(GL_ARRAY_BUFFER, _->_id[VBO]);
	glBufferData(GL_ARRAY_BUFFER, noVtc * sizeof (GLfloat), vtc, GL_STATIC_DRAW);

	glGenBuffers(1, &_->_id[IBO]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _->_id[IBO]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, noIdc * sizeof (GLushort), idc, GL_STATIC_DRAW);

	return _;
}
