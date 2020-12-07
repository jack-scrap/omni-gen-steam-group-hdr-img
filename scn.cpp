#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"

unsigned int consoleWd = state::ln * state::sz[0];

Disp disp("asdf", {
	state::view[0] + consoleWd, state::view[1]
}, col[false]);

Obj child[] = {
	objMk("wheel", false, glm::vec3(0.0, 0.0, 0.0))
};

Obj front = objMk("truck/front", true, child, sizeof child / sizeof *child, glm::vec3(0.0, 0.0, 0.0));
