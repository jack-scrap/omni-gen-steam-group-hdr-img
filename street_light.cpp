#include <cstdlib>

#include "street_light.h"
#include "layout.h"

StreetLight* streetLightMk(CArr pass, glm::vec3 loc, glm::vec3 rot) {
	StreetLight* _ = (StreetLight*) malloc(sizeof (StreetLight));

	_->_pass = pass;

	Obj* child[_->_pass._sz];
	for (int i = 0; i < _->_pass._sz; i++) {
		child[i] = objMk("street_light/go", "obj", "solid", true, glm::vec3(-((0.2 / 2) + (layout::pad * 2)), 6.0 + 1.0, i * 2.0), glm::vec3(0.0, M_PI / 2, 0.0));
	}

	_->_parent = objMk("street_light/body", "obj", "dir", false, child, sizeof child / sizeof *child, loc, rot);

	return _;
}

void streetLightToggle(StreetLight* streetLight, unsigned int i) {
	((bool*) streetLight->_pass._ptr)[i] = !((bool*) streetLight->_pass._ptr)[i];

	streetLight->_parent->_child[i]->_active = ((char*) streetLight->_pass._ptr)[i];
}
