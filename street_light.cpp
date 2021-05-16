#include <cstdlib>

#include "street_light.h"

StreetLight* streetLightMk(bool* pass, unsigned int no, glm::vec3 loc, glm::vec3 rot) {
	StreetLight* _ = (StreetLight*) malloc(sizeof (StreetLight));

	_->_pass = (bool*) malloc(no * sizeof (bool));
	for (int i = 0; i < no; i++) {
		_->_pass[i] = pass[i];
	}

	Obj* child[no];
	for (int i = 0; i < no; i++) {
		child[i] = objMk("go", "obj", "solid", true, glm::vec3(i * 2.0, 6.0, 0.0), glm::vec3(0.0, M_PI / 2, 0.0));
	}

	_->_parent = objMk("street_light", "obj", "dir", false, child, sizeof child / sizeof *child, loc, rot);

	return _;
}
