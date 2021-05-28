#pragma once

#include <GL/glew.h>

typedef struct Mesh {
	GLuint _id[4];

	unsigned int _noPrim;

	enum id {
		VAO,

		VBO,
		STBO,

		IBO
	};
} Mesh;

Mesh* meshMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noidc);
