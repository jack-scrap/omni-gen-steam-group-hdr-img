#include <iostream>

#include "scn.h"
#include "col.h"

Disp disp("asdf", {
	800, 600
}, col[false]);

GLfloat loc[2] = {
	0.0, 0.0
};
Poly tri = polyMk(loc);
