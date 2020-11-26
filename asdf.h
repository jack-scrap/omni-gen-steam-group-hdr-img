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

Disp disp("asdf", 800, 600);

Asdf asdfMk();

void asdfDraw(Asdf* asdf);

extern "C" void set(Asdf* asdf, int hjkl);
