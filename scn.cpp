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
		-1000.0, 1000.0, -1000.0
	}, {
		50, 50, 50
	}, {
		-1000.0, 1000.0, -1000.0
	}
};

Disp disp("Omni", {
	state::view[X] + (state::ln * state::dim[X]), state::view[Y]
}, col[false]);

Truck* truck = truckMk();

Crane* crane = craneMk();

std::vector<GLfloat> vtc = util::mesh::quad::pos(glm::vec2(
	state::pad * 2,
	100.0
));

std::vector<GLushort> idc = {
	0, 1, 2, 3
};
std::vector<GLushort> strip = util::mesh::strip(idc);

Obj* rng[2] = {
	objMk(&vtc[0], vtc.size() * sizeof (GLfloat), &strip[0], strip.size() * sizeof (GLushort), "obj", "solid", true, glm::vec3(0.0, 0.0, -(11.0 + (state::pad * 2) + (state::pad * 2) + (state::pad * 2) + (state::pad * 2))), glm::vec3(M_PI / 2, 0.0, M_PI / 2)),
	objMk(&vtc[0], vtc.size() * sizeof (GLfloat), &strip[0], strip.size() * sizeof (GLushort), "obj", "solid", true, glm::vec3(0.0, 0.0, 11.0 + (state::pad * 2) + (state::pad * 2)), glm::vec3(M_PI / 2, 0.0, M_PI / 2))
};

std::vector<Obj*> obj = {
	crane->_parent,
	objMk("container_2x4", "obj", "dir", true),
	/* rng[MIN], */
	/* rng[MAX] */
};

Bound* bound = boundMk(obj);

Truck* truckGet() {
	return truck;
}

Crane* craneGet() {
	return crane;
}
