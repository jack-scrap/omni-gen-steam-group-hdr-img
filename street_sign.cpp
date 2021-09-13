#include <cstdlib>

#include "street_sign.h"
#include "layout.h"

StreetSign* streetSignMk(CArr pass, glm::vec3 loc, glm::vec3 rot) {
	StreetSign* _ = (StreetSign*) malloc(sizeof (StreetSign));

	_->_pass = pass;

	Obj* child[_->_pass._sz];
	for (int i = 0; i < _->_pass._sz; i++) {
		child[i] = objMk("street_sign/go", "obj", "solid", true, glm::vec3((0.2 / 2) + (layout::pad * 2), 6.0 + 1.0, i * 2.0), glm::vec3(0.0, M_PI / 2, 0.0));
	}

	_->_parent = objMk("street_sign/body", "obj", "dir", false, child, sizeof child / sizeof *child, loc, rot);

	return _;
}

void streetSignDel(StreetSign* streetSign) {
	objDel(streetSign->_parent);

	free(streetSign);
}

void streetSignToggle(StreetSign* streetSign, unsigned int i) {
	((bool*) streetSign->_pass._ptr)[i] = !((bool*) streetSign->_pass._ptr)[i];

	streetSign->_parent->_child[i]->_active = ((char*) streetSign->_pass._ptr)[i];
}
