#include <cstdlib>

#include "street_light.h"

StreetLight* streetLightMk(bool* pass, unsigned int no, glm::vec3 loc, glm::vec3 rot) {
	StreetLight* _ = (StreetLight*) malloc(sizeof (StreetLight));

	_->_pass = (bool*) malloc(no * sizeof (bool));
	for (int i = 0; i < no; i++) {
		_->_pass[i] = pass[i];
	}

	_->_parent = objMk("street_light", "obj", "dir", true, loc, rot);

	return _;
}
