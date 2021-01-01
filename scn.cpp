#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"
#include "crane.h"

unsigned int consoleWd = state::ln * state::dim[X];

Cam cam = {
	{
		0, 0, 0
	}, {
		50, 50, 50
	}
};

Disp disp("asdf", {
	state::view[X] + consoleWd, state::view[Y]
}, col[false]);

Obj* wheel = objMk("wheel", "main", "dir", true, glm::vec3(3.0, 0.0, 0.0));

Truck* truck = truckMk(glm::vec3(0.0, 0.0, 0.0));

Crane* crane = craneMk(glm::vec3(0.0, 0.0, 0.0));

GLfloat vtc[] = {
	// back
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	-1.0, 1.0, -1.0,

	-1.0, 1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0, 1.0, -1.0,

	// front
	-1.0, -1.0, 1.0,
	1.0, -1.0, 1.0,
	-1.0, 1.0, 1.0,

	-1.0, 1.0, 1.0,
	1.0, -1.0, 1.0,
	1.0, 1.0, 1.0,


	// left
	-1.0, -1.0, -1.0,
	-1.0, -1.0, 1.0,
	-1.0, 1.0, -1.0,

	-1.0, 1.0, -1.0,
	-1.0, -1.0, 1.0,
	-1.0, 1.0, 1.0,

	// right
	1.0, -1.0, -1.0,
	1.0, -1.0, 1.0,
	1.0, 1.0, -1.0,

	1.0, 1.0, -1.0,
	1.0, -1.0, 1.0,
	1.0, 1.0, 1.0,


	// bottom
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	-1.0, -1.0, 1.0,

	-1.0, -1.0, 1.0,
	1.0, -1.0, -1.0,
	1.0, -1.0, 1.0,

	// top
	-1.0, 1.0, -1.0,
	1.0, 1.0, -1.0,
	-1.0, 1.0, 1.0,

	-1.0, 1.0, 1.0,
	1.0, 1.0, -1.0,
	1.0, 1.0, 1.0
};
GLushort idc[] = {
	0, 1, 2,
	3, 4, 5,
	6, 7, 8,
	9, 10, 11,
	12, 13, 14,
	15, 16, 17,
	18, 19, 20,
	21, 22, 23,
	24, 25, 26,
	27, 28, 29,
	30, 31, 32,
	33, 34, 35
};
Obj* cube = objMk(vtc, sizeof vtc / sizeof *vtc, idc, sizeof idc / sizeof *idc, "tex", "tex", true);
