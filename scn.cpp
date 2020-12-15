#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"

unsigned int consoleWd = state::ln * state::sz[0];

Disp disp("asdf", {
	state::view[0] + consoleWd, state::view[1]
}, col[false]);

Obj* wheel = objMk("wheel", "main", "dir", true, glm::vec3(3.0, 0.0, 0.0));

Truck* truck = truckMk();

Pt* pt = ptMk();
