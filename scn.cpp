#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"

unsigned int consoleWd = state::ln * state::sz[0];

Disp disp("asdf", {
	state::view[0] + consoleWd, state::view[1]
}, col[false]);

Obj wheel0 = objMk("wheel", false, glm::vec3(0.0, 0.0, 0.0));
Obj wheel1 = objMk("wheel", false, glm::vec3(0.0, 0.0, 0.0));
Obj wheel2 = objMk("wheel", false, glm::vec3(0.0, 0.0, 0.0));
Obj wheel3 = objMk("wheel", false, glm::vec3(0.0, 0.0, 0.0));

Obj child[] = {
	wheel0,
	wheel1,
	wheel2,
	wheel3
};

Obj front = objMk("truck/front", true, child, sizeof child / sizeof *child, glm::vec3(0.0, 0.0, 0.0));
