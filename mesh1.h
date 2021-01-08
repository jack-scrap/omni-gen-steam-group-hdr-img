#pragma once

#include <GL/glew.h>

enum id {
	VAO,
	VBO,
	STBO,
	IBO
};

typedef struct Mesh1 {
	GLuint _id[4];

	unsigned int _noIdc;
} Mesh1;

Mesh1* mesh1Mk(GLfloat* vtc, unsigned int noVtc, GLushort* idc, unsigned int noidc);
