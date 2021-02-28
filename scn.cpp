#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <chrono>

#include "scn.h"
#include "layout.h"
#include "col.h"
#include "crane.h"
#include "node.h"
#include "arr.h"

Cam cam = {
	{
		-1000.0, 1000.0, 1000.0
	}, {
		50, 50, 50
	}, {
		-1000.0, 1000.0, -1000.0
	}
};

Disp disp("Omni", {
	layout::view[X] + (state::ln * layout::dim[X]), layout::view[Y]
}, col[false]);

Truck* truck = truckMk();

Crane* crane = craneMk();

std::vector<GLfloat> vtc = util::mesh::quad::pos(glm::vec2(
	layout::pad,
	100.0
));

std::vector<GLushort> idc = {
	0, 1, 2, 3
};
std::vector<GLushort> strip = util::mesh::strip(idc);

Obj* rng[2] = {
	objMk(&vtc[0], vtc.size() * sizeof (GLfloat), &strip[0], strip.size() * sizeof (GLushort), "obj", "solid", true, glm::vec3(0.0, 0.0, -layout::stroke + -(10.0 + (layout::stroke * 2 * 2) + layout::stroke)), glm::vec3(M_PI / 2, 0.0, M_PI / 2)),
	objMk(&vtc[0], vtc.size() * sizeof (GLfloat), &strip[0], strip.size() * sizeof (GLushort), "obj", "solid", true, glm::vec3(0.0, 0.0, -layout::stroke + 10.0 + (layout::stroke * 2 * 2) + layout::stroke), glm::vec3(M_PI / 2, 0.0, M_PI / 2))
};

char asdf[] = {
	0,
	97,
	97
};

Node* child[] = {
	nodeMk(asdf, sizeof asdf / sizeof *asdf)
};

Node* data = nodeMk(asdf, sizeof asdf / sizeof *asdf, child, sizeof child / sizeof *child);
Arr* arr = arrMk(data);

std::vector<Obj*> scn = {
	crane->_parent,
	arr->_parent,
	rng[MIN],
	rng[MAX]
};

Truck* truckGet() {
	return truck;
}

Crane* craneGet() {
	return crane;
}

Obj* contGet() {
	return scn[1];
}

bool fin = false;
