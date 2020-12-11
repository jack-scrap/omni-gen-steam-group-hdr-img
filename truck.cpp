#include "truck.h"

Truck* truckMk() {
	Truck* _ = (Truck*) malloc(sizeof (Truck));

	Obj* child[2 * 2];
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			child[i] = objMk("wheel", "main", "dir", false, glm::vec3(x * -2.6, -1, z ? 1 : -1));

			i++;
		}
	}

	_->_bed = ptMk();

	_->_parent = objMk("truck/front", "main", "dir", true, child, sizeof child / sizeof *child, glm::vec3(2.4, 1.3, 0.0));

	return _;
}

void truckDraw(Truck* truck) {
	objDraw(truck->_parent);

	ptDraw(truck->_bed);
}
