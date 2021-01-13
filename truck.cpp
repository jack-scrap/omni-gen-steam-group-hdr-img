#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "truck.h"
#include "scn.h"
#include "state.h"

Truck* truckMk(glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	Obj* child[(2 * 2 * 2) + 1];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[i] = objMk("wheel", "obj", "dir", false, loc + glm::vec3(x * -2.6, -1, z ? 1 : -1), rot);

			i++;
		}
	}

	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[i] = objMk("rim", "obj", "dir", true, loc + glm::vec3(x * -2.6, -1, z ? 1 : -1), rot);

			i++;
		}
	}

	child[2 * 2 * 2] = objMk("truck/tail", "obj", "dir", false, loc + glm::vec3(-3.84, 0.0, 0.0), rot);

	_->_bed = ptMk(loc + glm::vec3(0.0, 0.2, 0.0), rot);

	_->_parent = objMk("truck/front", "tex", "tex", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	return _;
}
