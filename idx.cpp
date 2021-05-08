#include <cstdlib>
#include <string>

#include "idx.h"
#include "layout.h"
#include "str.h"

Idx* idxMk(unsigned int i, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	_->_data = nullptr;

	Obj* child[2];

	Obj* no = objMk("glyph/" + std::to_string(i), "obj", "solid", true, glm::vec3((layout::stroke * 2) + (layout::idx[X] / 2), 0.0, (layout::stroke * 2) + (layout::idx[Y] / 2)), rot);

	child[0] = no;

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[1] = id->_parent;
	} else {
		child[1] = nullptr;
	}

	Border* border = borderMk({
		layout::idx[X] + (layout::margin * 2 * 2), layout::idx[Y] + (layout::pad * 2 * 2)
	}, child, sizeof child / sizeof *child,
	loc, rot);
	_->_parent = border->_parent;

	return _;
}

Idx* idxMk(unsigned int i, char c, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	Obj* child[1 + 1 + 1];

	Obj* no = objMk("glyph/" + std::to_string(i), "obj", "solid", true, glm::vec3((layout::stroke * 2) + (layout::idx[X] / 2), 0.0, (layout::stroke * 2) + (layout::idx[Y] / 2)), rot);

	child[0] = no;

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[1] = id->_parent;
	} else {
		child[1] = nullptr;
	}

	// data
	Cont* byte = contMk(c, glm::vec3((layout::stroke * 2) + (2.0 / 2), 1.0, (layout::stroke * 2) + (4.0 / 2)));

	child[2] = byte->_parent;

	_->_data = byte;

	Border* border = borderMk({
		layout::idx[X] + (layout::margin * 2 * 2), layout::idx[Y] + (layout::pad * 2 * 2)
	}, child, sizeof child / sizeof *child, loc, rot);
	_->_parent = border->_parent;

	return _;
}

void idxInsert(Idx* idx, Cont* byte) {
	idx->_data = byte;
	idx->_parent->_child[idx->_parent->_noChild - 1] = idx->_data->_parent;

	objAcc(idx->_parent, glm::mat4(1.0));
}

Cont* idxPop(Idx* idx) {
	Cont* byte = idx->_data;

	idx->_data = nullptr;
	idx->_parent->_child[idx->_parent->_noChild - 1] = idx->_data->_parent;

	return byte;
}

bool idxEq(Idx* lhs, Idx* rhs) {
	bool _;

	// null
	if (!lhs->_data && !rhs->_data) {
		_ = true;
	}

	if (!lhs->_data ^ !rhs->_data) {
		_ = false;
	}

	if (lhs->_data && rhs->_data) {
		_ = lhs->_data->_c == rhs->_data->_c;
	}

	return _;
}
