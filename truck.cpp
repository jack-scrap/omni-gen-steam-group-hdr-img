#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "truck.h"
#include "scn.h"

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

void truckMv(Truck* truck, GLfloat* d) {
	for (int i = 0; i < 3; i++) {
		truck->_parent->_loc[i] += d[i];
	}

	truck->_parent->_model = glm::scale(truck->_parent->_model, glm::vec3(50));
	truck->_parent->_model = glm::translate(truck->_parent->_model, glm::vec3(d[0], d[1], d[2]));

	for (int i = 0; i < truck->_parent->_noChild; i++) {
		if (&truck->_parent->_child[i]) {
			truck->_parent->_child[i]->_model = glm::translate(truck->_parent->_child[i]->_model, glm::vec3(d[0], d[1], d[2]));
			truck->_parent->_model = glm::scale(truck->_parent->_model, glm::vec3(50));
		}
	}
}

Truck* truckGet() {
	return truck;
}

void truckSet(Truck* truck, GLfloat* d) {
	unsigned int fps = 10;

	float step[3];
	for (int i = 0; i < 3; i++) {
		step[i] = d[i] / fps;
	}

	for (int t = 0; t < fps; t++) {
		for (int i = 0; i < 3; i++) {
			truck->_parent->_loc[i] += step[i];
		}

		truck->_parent->_model = glm::translate(glm::mat4(1.0), truck->_parent->_loc);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
	}
}

void truckDraw(Truck* truck) {
	objDraw(truck->_parent);

	ptDraw(truck->_bed);
}
