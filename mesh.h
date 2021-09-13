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

	enum type {
		PT,
		LINE,
		OBJ
	};
} Mesh;

Mesh* meshMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim);

void meshDel(Mesh* mesh);
