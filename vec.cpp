#include <cstdlib>
#include <vector>

#include "vec.h"

Vec* vecMk(glm::vec3 loc, glm::vec3 rot) {
	Vec* _ = (Vec*) malloc(sizeof (Vec));

	_->_parent = objMk("vec/front", "obj", "dir", true, loc, rot);

	return _;
}
