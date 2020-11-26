#pragma once

#include <cstdlib>
#include <GL/glew.h>

#include "disp.h"

typedef struct {
	int _asdf;

	GLuint
		_vao,
		_vbo;
} Asdf;

Asdf asdfMk();

extern "C" void asdfDraw(Asdf* asdf);

extern "C" void set(Asdf* asdf, int hjkl);
