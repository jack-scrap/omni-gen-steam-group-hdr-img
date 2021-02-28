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

	Obj* child[] = {
		objMk("glyph/" + std::to_string(i), "main", "solid", true, glm::vec3(
			(layout::idx[X] / 2) + layout::stroke + (layout::margin * 2), 0.0, (layout::idx[Y] / 2) + layout::stroke + (layout::margin * 2)
		), rot)
	};

	_->_parent->_noChild++;
	_->_parent->_child = (Obj**) realloc(_->_parent->_child, _->_parent->_noChild * sizeof (Obj*));
	for (int i = 0; i < _->_parent->_noChild; i++) {
		_->_parent->_child[i] = child[i];
	}

	return _;
}
