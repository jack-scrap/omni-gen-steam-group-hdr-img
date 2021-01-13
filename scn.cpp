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

Truck* truck = truckMk(glm::vec3(0.0, 0.0, 0.0));

Bound* bound = boundMk();

Crane* crane = craneMk(glm::vec3(0.0, 0.0, 0.0));

std::vector<Obj*> obj = {
	bound->_parent
};

Truck* truckGet() {
	return truck;
}

void truckSet(Truck* truck, GLfloat* d) {
	/* float step[3]; */
	/* for (int i = 0; i < 3; i++) { */
	/* 	step[i] = d[i] / state::fps; */
	/* } */

	/* for (int t = 0; t < state::fps; t++) { */
	/* 	for (int i = 0; i < 3; i++) { */
	/* 		truck->_parent->_loc[i] += step[i]; */
	/* 	} */

	/* 	truck->_parent->_model = glm::translate(glm::mat4(1.0), truck->_parent->_loc); */

	/* 	std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps)); */
	/* } */
}

Crane* craneGet() {
	return crane;
}

void craneSet(Crane* crane, GLfloat* d) {
	/* GLfloat step[3]; */
	/* for (int i = 0; i < 3; i++) { */
	/* 	step[i] = d[i] / state::fps; */
	/* } */

	/* for (int t = 0; t < state::fps; t++) { */
	/* 	for (int i = 0; i < 3; i++) { */
	/* 		crane->_parent->_loc[i] += step[i]; */
	/* 	} */

	/* 	crane->_parent->_model = glm::translate(glm::mat4(1.0), crane->_parent->_loc); */

	/* 	std::this_thread::sleep_for(std::chrono::milliseconds(1000 / state::fps)); */
	/* } */
}
