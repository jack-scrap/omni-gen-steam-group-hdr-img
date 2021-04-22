#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "truck.h"
#include "scn.h"
#include "state.h"

Truck* truckMk(glm::vec3 loc, glm::vec3 rot) {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	Obj* child[(2 * 2) + 1];
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

	child[2 * 2] = objMk("truck/tail", "obj", "dir", true, glm::vec3(-3.84 - (0.32 * 2), 0.0, 0.0), rot);

	i++;

	_->_parent = objMk("truck/front", "obj", "dir", true, child, sizeof child / sizeof *child, loc + glm::vec3(2.4, 1.3, 0.0), rot);

	return _;
}

void truckTurn(Truck* truck, float delta) {
	objAnim(truck->_parent, glm::vec3(0.0), glm::vec3(0.0, M_PI, 0.0));
}

void truckZoom(Truck* truck, float delta) {
	glm::vec3 loc = glm::vec3(
		delta,
		0.0,
		0.0
	);

	objAnim(truck->_parent, loc, glm::vec3(0.0));
}
