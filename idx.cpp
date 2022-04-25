#include <cstdlib>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

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

	glm::vec2 center = layout::center({
		layout::idx[X],
		layout::idx[Z]
	});
	for (int i = 0; i < str.size(); i++) {
		child[i] = objMk("glyph/" + std::string(1, str[i]), "obj", "solid", true, glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(center[X], 0.0, center[Y]) + glm::vec3(i * (layout::stroke + layout::letter[X]), 0.0, 0.0), rot);
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[str.size()] = id->_parent;
	} else {
		child[str.size()] = nullptr;
	}

	Border* scope = borderMk({
		layout::item(layout::idx[X]),
		layout::item(layout::idx[Z])
	}, child, sizeof child / sizeof *child, loc, rot);
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

	glm::vec2 center = layout::center({
		layout::idx[X],
		layout::idx[Z]
	});
	for (int i = 0; i < str.size(); i++) {
		child[i] = objMk("glyph/" + std::string(1, str[i]), "obj", "solid", true, glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(center[X], 0.0, center[Y]) + glm::vec3(i * (layout::stroke + layout::letter[X]), 0.0, 0.0), rot);
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[1] = id->_parent;
	} else {
		child[1] = nullptr;
	}

	// data
	glm::vec2 stride = glm::vec2(layout::item(layout::bordered(layout::idx[X])), layout::item(layout::bordered(layout::idx[Z])));

	for (int i = 0; i < _->_sz; i++) {
		Cont* byte = contMk(c[i], glm::vec3((layout::stroke * 2) + (layout::idx[X] / 2), 1.0, (layout::stroke * 2) + (layout::idx[Z] / 2)) + glm::vec3(0.0, i * layout::idx[Y], 0.0));

		_->_data = byte;

		child[2 + i] = byte->_parent;
	}

	Border* scope = borderMk({
		layout::item(layout::idx[X]),
		layout::item(layout::idx[Z])
	}, child, sizeof child / sizeof *child, loc, rot);
	_->_parent = scope->_parent;

	return _;
}

void idxDel(Idx* idx) {
	if (idx->_data) {
		contDel(idx->_data);
	}

	free(idx);
}

void idxInsert(Idx* idx, Cont* byte) {
	idx->_data = byte;
	idx->_parent->_child[idx->_parent->_noChild - 1] = idx->_data->_parent;

	// transform
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(layout::bordered(layout::idx[X]), layout::idx[Y], layout::bordered(layout::idx[Z])) / glm::vec3(2));

	idx->_data->_parent->_model = model;

	objAcc(idx->_parent, glm::mat4(1.0));
}

Cont* idxMv(Idx* idx) {
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
