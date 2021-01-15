#pragma once

#include <GL/glew.h>

enum id {
	VAO,
	VBO,
	STBO,
	IBO
};

typedef struct Mesh {
	GLuint _id[4];

	unsigned int _noIdc;
} Mesh;

Mesh* meshMk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noidc);
