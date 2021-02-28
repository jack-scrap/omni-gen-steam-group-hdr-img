#include <cstdlib>
#include <string>

#include "idx.h"
#include "layout.h"

Idx* idxMk(unsigned int i, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	Border* border = borderMk({
		layout::idx[X] + (layout::margin * 2 * 2), layout::idx[Y] + (layout::margin * 2 * 2)
	}, loc, rot);
	_->_parent = border->_parent;

	return _;
}

Idx* idxMk(unsigned int i, char c, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	Obj* child[1] = {
		objMk("container_2x4", "obj", "dir", true, glm::vec3((layout::stroke * 2) + (2.0 / 2), 1.0, (layout::stroke * 2) + (4.0 / 2)))
	};

	Border* border = borderMk({
		layout::idx[X] + (layout::margin * 2 * 2), layout::idx[Y] + (layout::margin * 2 * 2)
		}, child, 1, loc, rot);
	_->_parent = border->_parent;

	return _;
}
