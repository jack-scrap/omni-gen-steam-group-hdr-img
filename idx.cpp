#include <cstdlib>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "idx.h"
#include "layout.h"
#include "str.h"
#include "util.h"

Idx* idxMk(unsigned int i, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Idx* _ = (Idx*) malloc(sizeof (Idx));

	_->_data = (Cont**) malloc(0);
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
	child[str.size()] = nullptr;

	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[str.size()] = id->_parent;
	}

	Border* border = borderMk({
		layout::item(layout::idx[X]),
		layout::item(layout::idx[Z])
	}, child, sizeof child / sizeof *child, loc, rot);
	_->_parent = border->_parent;

	for (int i = 0; i < str.size(); i++) {
		glm::vec3 center = layout::center(child[i]);

		child[i]->_model = glm::translate(child[i]->_model, glm::vec3(center[X], 0.0, -center[Z]));
	}

	objAcc(_->_parent, glm::mat4(1.0));

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		_->_offset[a] = offset[a];
	}

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
	child[1] = nullptr;

	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[1] = id->_parent;
	}

	// data
	glm::vec2 stride = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	for (int i = 0; i < _->_sz; i++) {
		Cont* byte = contMk(c[i], glm::vec3((layout::stroke * 2) + (layout::idx[X] / 2), layout::idx[Y] / 2, (layout::stroke * 2) + (layout::idx[Z] / 2)) + glm::vec3(0.0, i * layout::idx[Y], 0.0));

		_->_data[i] = byte;

		child[2 + i] = byte->_parent;
	}

	Border* border = borderMk({
		layout::item(layout::idx[X]),
		layout::item(layout::idx[Z])
	}, child, sizeof child / sizeof *child, loc, rot);
	_->_parent = border->_parent;

	for (int i = 0; i < str.size(); i++) {
		glm::vec3 center = layout::center(child[i]);

		objMv(child[i], _->_parent, glm::vec3(center[X], 0.0, -center[Z]), glm::vec3(0.0));
	}

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		_->_offset[a] = offset[a];
	}

	return _;
}

void idxDel(Idx* inst) {
	for (int i = 0; i < inst->_sz; i++) {
		if (inst->_data[i]) {
			contDel(inst->_data[i]);
		}
	}

	free(inst);
}

void idxPush(Idx* inst, Cont* byte) {
	inst->_sz++;
	inst->_data = (Cont**) realloc(inst->_data, inst->_sz * sizeof (Cont*));

	inst->_parent->_noChild++;
	inst->_parent->_child = (Obj**) realloc(inst->_parent->_child, inst->_parent->_noChild * sizeof (Cont*));

	inst->_data[inst->_sz - 1] = byte;
	inst->_parent->_child[inst->_parent->_noChild - 1] = inst->_data[inst->_sz - 1]->_parent;

	// transform
	glm::vec2 center = layout::center({
		layout::idx[X],
		layout::idx[Z]
	});
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(center[X], 0.0, center[Y]) + glm::vec3(0.0, layout::idx[Y] / 2, 0.0));

	inst->_data[inst->_sz - 1]->_parent->_model = model;

	objAcc(inst->_parent, glm::mat4(1.0));
}

Cont* idxPop(Idx* inst) {
	Cont* byte = nullptr;

	if (inst->_sz) {
		byte = inst->_data[inst->_sz - 1];

		inst->_sz--;
		inst->_data = (Cont**) realloc(inst->_data, inst->_sz * sizeof (Cont*));

		inst->_parent->_noChild--;
		inst->_parent->_child = (Obj**) realloc(inst->_parent->_child, inst->_parent->_noChild * sizeof (Cont*));
	}

	return byte;
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
