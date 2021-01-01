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

Obj* wheel = objMk("wheel", "main", "dir", true, glm::vec3(3.0, 0.0, 0.0));

Truck* truck = truckMk(glm::vec3(0.0, 0.0, 0.0));

Crane* crane = craneMk(glm::vec3(0.0, 0.0, 0.0));
