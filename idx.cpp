#include <cstdlib>
#include <string>

#include "idx.h"
#include "layout.h"
#include "str.h"

Idx* idxMk(unsigned int i, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	_->_data = nullptr;
	_->_sz = 0;
	_->_i = i;

	// index
	std::string str = std::to_string(i);

	Obj* child[str.size() + 1];

	glm::vec3 overhead = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2);
	glm::vec3 center = glm::vec3(layout::idx[X] / 2, 0.0, layout::idx[Z] / 2) + (glm::vec3(str.size() - 1) * glm::vec3(-((layout::letter[X] + (layout::margin * 2 * 2)) / 2), 0.0, 0.0));
	for (int i = 0; i < str.size(); i++) {
		glm::vec3 offset = glm::vec3(i * (layout::stroke + layout::letter[X]), 0.0, 0.0);
		child[i] = objMk("glyph/" + std::string(1, str[i]), "obj", "solid", true, overhead + center + offset, rot);
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[str.size()] = id->_parent;
	} else {
		child[str.size()] = nullptr;
	}

	Border* scope = borderMk(layout::margined(layout::margined({
		layout::idx[X],
		layout::idx[Z]
	})), child, sizeof child / sizeof *child, loc, rot);
	_->_parent = scope->_parent;

	return _;
}

Idx* idxMk(unsigned int i, char* c, unsigned int sz, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	_->_sz = sz;
	_->_i = i;

	// index
	std::string str = std::to_string(i);

	Obj* child[str.size() + _->_sz + 1];

	glm::vec3 overhead = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2);
	glm::vec3 center = glm::vec3(layout::idx[X] / 2, 0.0, layout::idx[Z] / 2);
	for (int i = 0; i < str.size(); i++) {
		glm::vec3 offset = glm::vec3(i * (layout::stroke + layout::letter[X]), 0.0, 0.0);
		child[i] = objMk("glyph/" + std::string(1, str[i]), "obj", "solid", true, overhead + center + offset, rot);
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[1] = id->_parent;
	} else {
		child[1] = nullptr;
	}

	// data
	for (int i = 0; i < _->_sz; i++) {
		Cont* byte = contMk(c[i], glm::vec3((layout::stroke * 2) + (layout::idx[X] / 2), 1.0, (layout::stroke * 2) + (layout::idx[Z] / 2)) + glm::vec3(0.0, i * layout::stride[Y], 0.0));

		_->_data = byte;

		child[2 + i] = byte->_parent;
	}

	Border* scope = borderMk(layout::margined(layout::margined({
		layout::idx[X],
		layout::idx[Z]
	})), child, sizeof child / sizeof *child, loc, rot);
	_->_parent = scope->_parent;

	return _;
}

void idxDel(Idx* idx) {
	contDel(idx->_data);

	free(idx);
}

void idxInsert(Idx* idx, Cont* byte) {
	idx->_data = byte;
	idx->_parent->_child[idx->_parent->_noChild - 1] = idx->_data->_parent;

	objAcc(idx->_parent, glm::mat4(1.0));
}

Cont* idxPop(Idx* idx) {
	Cont* _ = idx->_data;

	idx->_data = nullptr;
	idx->_parent->_child[idx->_parent->_noChild - 1] = nullptr;

	return _;
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
