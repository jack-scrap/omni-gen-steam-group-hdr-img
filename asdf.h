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

extern "C" Disp disp("asdf", 800, 600);

extern "C" Asdf asdfMk();

extern "C" void asdfDraw(Asdf* asdf);

extern "C" void set(Asdf* asdf, int hjkl);
