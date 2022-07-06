#include <cstdlib>

#include "array.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"
#include "util.h"

Array* arrayMk(char* init, unsigned int x, std::string name, unsigned int axis, glm::vec3 loc, glm::vec3 rot) {
	Array* inst = (Array*) malloc(sizeof (Array));

	inst->_x = x;
	inst->_y = 1;

	// data
	inst->_data = (Idx**) malloc(inst->_x * inst->_y * sizeof (Idx*));

	unsigned int noChild = 1 + (inst->_x * inst->_y);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	glm::vec2 stride = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	int c = 0;
	for (int i = 0; i < inst->_x; i++) {
		Idx* idx;

		glm::vec3 offset;
		switch (axis) {
			case X:
				offset = glm::vec3(c * stride[X], 0.0, 0.0);

				break;

			case Z:
				offset = glm::vec3(0.0, 0.0, c * stride[Y]);

				break;
		}

		if (init[c]) {
			idx = idxMk(c, &init[c], 1, "", glm::vec3(layout::overhead, 0.0, layout::overhead) + offset);
		} else {
			idx = idxMk(c, "", glm::vec3(layout::overhead, 0.0, layout::overhead) + offset);
		}

		inst->_data[c] = idx;
		child[1 + c] = inst->_data[c]->_parent;

		c++;
	}

	// identifier
	child[0] = nullptr;

	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
}

	glm::vec2 bound;
	switch (axis) {
		case X:
			bound = glm::vec2(inst->_x, inst->_y);

			break;

		case Z:
			bound = glm::vec2(inst->_y, inst->_x);

			break;
	}

	Border* scope;
	switch (axis) {
		case X:
			scope = borderMk({
				inst->_x * stride[X],
				inst->_y * stride[Y]
			}, child, noChild, loc, rot);

			break;

		case Z:
			scope = borderMk({
				inst->_y * stride[X],
				inst->_x * stride[Y]
			}, child, noChild, loc, rot);

			break;
	}

	inst->_parent = scope->_parent;

	// offset
	glm::vec3 offset = inst->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		inst->_offset[a] = offset[a];
	}

	return inst;
}

Array* arrayMk(char* init, unsigned int x, unsigned int y, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Array* _ = (Array*) malloc(sizeof (Array));

	_->_x = x;
	_->_y = y;

	unsigned int noChild = 1 + (_->_x * _->_y);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	// identifier
	child[0] = nullptr;

	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
	}

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * sizeof (Idx*));

	glm::vec2 stride = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	int c = 0;
	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;

			glm::vec3 offset = glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(i * stride[X], 0.0, j * stride[Y]);

			if (init[c]) {
				idx = idxMk(c, &init[c], 1, "", offset);
			} else {
				idx = idxMk(c, "", offset);
			}

			_->_data[c] = idx;
			child[1 + c] = _->_data[c]->_parent;

			c++;
		}
	}

	Border* scope = borderMk({
		_->_x * stride[X],
		_->_y * stride[Y]
	}, child, noChild, loc, rot);

	_->_parent = scope->_parent;

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		_->_offset[a] = offset[a];
	}

	return _;
}

void arrayDel(Array* inst) {
	for (int y = 0; y < inst->_y; y++) {
		for (int x = 0; x < inst->_x; x++) {
			idxDel(inst->_data[(y * inst->_x) + x]);
		}
	}
	free(inst->_data);

	objDel(inst->_parent);

	free(inst);
}

void arrayPush(Array* inst, unsigned int x, unsigned int y, Cont* byte) {
	int i = util::math::idx::array({
		x,
		y
	}, {
		inst->_x,
		inst->_y
	});

	Idx* idx = inst->_data[i];

	idxPush(idx, byte);

	// transform
	glm::vec2 center = layout::center({
		layout::idx[X],
		layout::idx[Z]
	});
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(center[X], 0.0, center[Y]) + glm::vec3(0.0, layout::idx[Y] / 2, 0.0));

	idx->_data[idx->_sz - 1]->_parent->_model = model;

	objAcc(idx->_parent, inst->_parent->_acc);
}

Cont* arrayPop(Array* inst, unsigned int x, unsigned int y) {
	Idx* idx = inst->_data[(y * inst->_x) + x];

	return idxPop(idx);
}

bool arrayEq(Array* lhs, Array* rhs) {
	int i = 0;
	for (int y = 0; y < lhs->_y; y++) {
		for (int x = 0; x < lhs->_x; x++) {
			if (!idxEq(lhs->_data[i], rhs->_data[i])) {
				return false;
			}

			i++;
		}
	}

	return true;
}
