#include <iostream>

#include "scn.h"
#include "state.h"
#include "col.h"
#include "crane.h"

Cam cam = {
	{
		1000.0, 1000.0, 1000.0
	}, {
		50, 50, 50
	}
};

Disp disp("Omni", {
	state::view[X] + (state::ln * state::dim[X]), state::view[Y]
}, col[false]);

Truck* truck = truckMk(glm::vec3(0.0, 0.0, 0.0));

Crane* crane = craneMk(glm::vec3(0.0, 0.0, 0.0));

std::vector<Obj*> obj = {
	objMk("container_2x4", "main", "dir", true)
};
