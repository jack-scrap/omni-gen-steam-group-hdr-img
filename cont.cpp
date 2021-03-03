#include "cont.h"
#include "layout.h"

Cont* contMk(char c, glm::vec3 loc, glm::vec3 rot) {
	Cont* _ = (Cont*) malloc(sizeof (Cont));

	_->_parent = objMk("container_2x4", "obj", "dir", true, loc, rot);

	_->_c = c;

	return _;
}
