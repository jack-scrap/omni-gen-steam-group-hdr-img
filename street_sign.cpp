#include <cstdlib>

#include "street_sign.h"
#include "layout.h"

StreetSign* streetSignMk(CArr pass, glm::vec3 loc, glm::vec3 rot) {
	StreetSign* inst = (StreetSign*) malloc(sizeof (StreetSign));

	inst->_pass = pass;

	Obj* child[inst->_pass.sz];
	for (int i = 0; i < inst->_pass.sz; i++) {
		child[i] = objMk("street_sign/go", "obj", "solid", true, glm::vec3(layout::item(0.2 / 2), 6.0 + 1.0, i * 2.0), glm::vec3(0.0, M_PI / 2, 0.0));
	}

	inst->_parent = objMk("street_sign/body", "obj", "dir", false, child, sizeof child / sizeof *child, loc, rot);

	return inst;
}

void streetSignDel(StreetSign* inst) {
	objDel(inst->_parent);

	free(inst);
}

void streetSignToggle(StreetSign* inst, unsigned int i) {
	((bool*) inst->_pass.ptr)[i] = !((bool*) inst->_pass.ptr)[i];

	inst->_parent->_child[i]->_active = ((char*) inst->_pass.ptr)[i];
}
