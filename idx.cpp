#include <cstdlib>
#include <string>

#include "idx.h"
#include "layout.h"

Idx* idxMk(unsigned int i, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	_->_data = nullptr;

	Obj* no = objMk("glyph/" + std::to_string(i), "obj", "solid", true, glm::vec3(
		(layout::stroke * 2) + (layout::idx[X] / 2),
		0.0,
		(layout::stroke * 2) + (layout::idx[Y] / 2)
	), rot);

	Obj* child[] = {
		no
	};

	Border* border = borderMk({
		layout::idx[X] + (layout::margin * 2 * 2), layout::idx[Y] + (layout::pad * 2 * 2)
	}, child, sizeof child / sizeof *child,
	loc, rot);
	_->_parent = border->_parent;

	return _;
}

Idx* idxMk(unsigned int i, char c, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	Cont* cont = contMk(c, glm::vec3((layout::stroke * 2) + (2.0 / 2), 1.0, (layout::stroke * 2) + (4.0 / 2)));

	Obj* child[1] = {
		cont->_parent
	};

	_->_data = cont;

	Border* border = borderMk({
		layout::idx[X] + (layout::margin * 2 * 2), layout::idx[Y] + (layout::pad * 2 * 2)
	}, child, 1, loc, rot);
	_->_parent = border->_parent;

	return _;
}
