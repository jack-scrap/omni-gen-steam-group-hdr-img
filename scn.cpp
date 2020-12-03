#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"

unsigned int consoleWd = state::ln * state::sz[0];

Disp disp("asdf", {
	800 + consoleWd, 600
}, col[false]);

glm::vec3 loc = glm::vec3(0.0, 0.0, 0.0);

Poly tri = polyMk(loc);

GLfloat vtc[] = {
	0.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0
};

GLushort idc[] = {
	0, 1, 2
};

Obj obj = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, true, loc);

Obj* child[] = {
	&obj
};

Obj obj1 = objMk("wheel", true, child, sizeof child / sizeof *child, loc);
