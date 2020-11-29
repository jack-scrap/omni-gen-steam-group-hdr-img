#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"

unsigned int consoleWd = state::ln * state::sz[0];

Disp disp("asdf", {
	800 + consoleWd, 600
}, col[false]);

GLfloat loc[3] = {
	0.0, 0.0, 0.0
};

Poly tri = polyMk(loc);

Obj obj = objMk("wheel", loc);
