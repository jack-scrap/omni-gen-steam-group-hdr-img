#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"
#include "crane.h"

Cam cam = {
	{
		0, 0, 0
	}, {
		50, 50, 50
	}
};

Disp disp("asdf", {
	state::view[X] + (state::ln * state::dim[X]), state::view[Y]
}, col[false]);

Truck* truck = truckMk(glm::vec3(0.0, 0.0, 0.0));

Crane* crane = craneMk(glm::vec3(0.0, 0.0, 0.0));

Obj* cont = objMk("container_2x4", "main", "dir", true, glm::vec3(0.0, 0.0, 0.0));
