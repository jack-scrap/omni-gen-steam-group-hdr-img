#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"

unsigned int consoleWd = state::ln * state::dim[X];

Disp disp("asdf", {
	state::view[X] + consoleWd, state::view[Y]
}, col[false]);

Obj* wheel = objMk("wheel", "main", "dir", true, glm::vec3(3.0, 0.0, 0.0));

Truck* truck = truckMk(glm::vec3(0.0, 0.0, 0.0));

GLfloat vtc[] = {
	// front
	-1.0, -1.0, 1.0,
	1.0, -1.0, 1.0,
	1.0,  1.0, 1.0,
	-1.0,  1.0, 1.0,
	// back
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0
};
GLushort idc[] = {
	// front
	0, 1, 2,
	2, 3, 0,
	// right
	1, 5, 6,
	6, 2, 1,
	// back
	7, 6, 5,
	5, 4, 7,
	// left
	4, 0, 3,
	3, 7, 4,
	// bottom
	4, 5, 1,
	1, 0, 4,
	// top
	3, 2, 6,
	6, 7, 3
};
Obj* cube = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "tex", "tex", true);
