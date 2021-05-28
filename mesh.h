#pragma once

#include <GL/glew.h>

typedef struct Mesh {
	GLuint _id[3];

	unsigned int _noPrim;

	enum id {
		VAO,

		VBO,

		IBO,

		STBO
	};
} Mesh;

Mesh* meshMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noPrim);
