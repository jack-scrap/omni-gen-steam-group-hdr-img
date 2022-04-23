#pragma once

#include <GL/glew.h>

typedef struct Mesh {
	GLuint _id[4];

	unsigned int _noPrim;

	enum {
		VAO,

		VBO,

		IBO,

		STBO
	};

	enum {
		PT,
		LINE,
		OBJ
	};
} Mesh;

Mesh* meshMk(GLfloat* vtc, GLushort* idc, unsigned int noPrim);

void meshDel(Mesh* mesh);
