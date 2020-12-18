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
