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
	_->_data = (Cont**) malloc(_->_sz * sizeof (Cont*));
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
		Cont* byte = contMk(c[i], glm::vec3((layout::stroke * 2) + (layout::idx[X] / 2), layout::idx[Y] / 2, (layout::stroke * 2) + (layout::idx[Z] / 2)) + glm::vec3(0.0, i * layout::idx[Y], 0.0));

		_->_data[i] = byte;

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
	for (int i = 0; i < idx->_sz; i++) {
		if (idx->_data[i]) {
			contDel(idx->_data[i]);
		}
	}

	free(idx);
}

void idxPush(Idx* idx, Cont* byte) {
	idx->_sz++;
	idx->_data = (Cont**) realloc(idx->_data, idx->_sz * sizeof (Cont*));

	idx->_parent->_noChild++;
	idx->_parent->_child = (Obj**) realloc(idx->_parent->_child, idx->_parent->_noChild * sizeof (Cont*));

	idx->_data[idx->_sz - 1] = byte;
	idx->_parent->_child[idx->_parent->_noChild - 1] = idx->_data[idx->_sz - 1]->_parent;

	// transform
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(glm::vec3(layout::bordered(layout::idx[X]), layout::idx[Y], layout::bordered(layout::idx[Z])) / glm::vec3(2)) + glm::vec3(0.0, (idx->_sz - 1) * layout::idx[Y], 0.0));

	idx->_data[idx->_sz - 1]->_parent->_model = model;

	objAcc(idx->_parent, glm::mat4(1.0));
}

Cont* idxPop(Idx* idx) {
	Cont* _;

	if (idx->_sz) {
		_ = idx->_data[idx->_sz - 1];

		idx->_sz--;
		idx->_data = (Cont**) realloc(idx->_data, idx->_sz * sizeof (Cont*));

		idx->_parent->_noChild--;
		idx->_parent->_child = (Obj**) realloc(idx->_parent->_child, idx->_parent->_noChild * sizeof (Cont*));

		idx->_parent->_child[2 + idx->_sz - 1] = nullptr;
	} else {
		_ = nullptr;
	}

	return _;
}

bool idxEq(Idx* lhs, Idx* rhs) {
	// null
	if (!lhs->_sz && !rhs->_sz) {
		return true;
	}

	if (!lhs->_sz ^ !rhs->_sz) {
		return false;
	}

	if (lhs->_sz != rhs->_sz) {
		return false;
	}

	if (lhs->_sz && rhs->_sz) {
		for (int i = 0; i < lhs->_sz; i++) {
			if (lhs->_data[i]->_c != rhs->_data[i]->_c) {
				return false;
			}
		}
	}
}
