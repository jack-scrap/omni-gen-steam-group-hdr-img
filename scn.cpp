#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <chrono>

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

Truck* truck = truckMk();

Crane* crane = craneMk();

Bound* bound = boundMk();

std::vector<Obj*> obj = {
	crane->_parent,
	objMk("container_2x4", "obj", "dir", true)
};

Truck* truckGet() {
	return truck;
}

Crane* craneGet() {
	return crane;
}
