#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "truck.h"
#include "scn.h"
#include "state.h"
#include "omni.h"

Truck* truckMk(glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_ptr = nullptr;
	_->_sz = 0;

	_->_ang = 0.0;

	Obj* child[(2 * 2) + 1 + 1];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			Obj* rim[] = {
				objMk("rim", "obj", "dir", true)
			};

			child[i] = objMk("wheel", "obj", "dir", false, rim, sizeof rim / sizeof *rim, glm::vec3(x * -2.6, -1, z ? 1 : -1), glm::vec3(0.0, z * M_PI, 0.0));

			i++;
		}
	}

	child[2 * 2] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-(2 * 4.0) - (layout::stroke) - (layout::stroke * 2), 0.0, 0.0), rot);

	child[(2 * 2) + 1] = nullptr;

	_->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	return _;
}

Truck* truckMk(char* data, unsigned int sz, glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	_->_ptr = data;
	_->_sz = sz;

	_->_ang = 0.0;

	Obj* child[(2 * 2) + sz + 1];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			Obj* rim[] = {
				objMk("rim", "obj", "dir", true)
			};

			child[i] = objMk("wheel", "obj", "dir", false, rim, sizeof rim / sizeof *rim, glm::vec3(x * -2.6, -1, z ? 1 : -1), glm::vec3(0.0, z * M_PI, 0.0));

			i++;
		}
	}

	child[2 * 2] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-(2 * 4.0) - (layout::stroke) - (layout::stroke * 2), 0.0, 0.0), rot);

	for (int i = 0; i < sz; i++) {
		child[(2 * 2) + 1 + i] = objMk("container_2x4", "obj", "dir", true, glm::vec3(-((i * (layout::idx[Y] + (layout::pad * 2))) + (layout::idx[Y] / 2) + 0.42), ((2.0 / 2) + 0.5), 0.0), glm::vec3(0.0, M_PI / 2, 0.0));
	}

	_->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	return _;
}

void truckTurn(Truck* truck, float delta) {
	if (
		truck->_ang + delta > truck->_rngWheel[MIN] &&
		truck->_ang + delta < truck->_rngWheel[MAX]
	) {
		truck->_ang += delta;

		for (int z = 0; z < 2; z++) {
			objAnim(truck->_parent->_child[z * 2], glm::vec3(0.0), glm::vec3(0.0, delta, 0.0));
		}
	} else {
		omni::err("Cannot turn truck; rotation of wheels exceeds range");
	}
}

void truckZoom(Truck* truck, float delta) {
	glm::vec3 loc = glm::vec3(delta, 0.0, 0.0);

	objAnim(truck->_parent, loc, glm::vec3(0.0));
}
