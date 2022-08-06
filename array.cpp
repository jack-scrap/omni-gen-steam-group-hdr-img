#include <cstdlib>

#include "array.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"
#include "util.h"

Array* arrayMk(char* data, unsigned int x, std::string name, unsigned int axis, glm::vec3 loc, glm::vec3 rot) {
	Array* inst = (Array*) malloc(sizeof (Array));

	inst->x = x;
	inst->y = 1;

	// data
	inst->data = (Idx**) malloc(inst->x * inst->y * sizeof (Idx*));

	unsigned int noChild = 1 + (inst->x * inst->y);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	glm::vec2 stride = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	int c = 0;
	for (int i = 0; i < inst->x; i++) {
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

		if (data[c]) {
			idx = idxMk(c, &data[c], 1, "", glm::vec3(layout::overhead, 0.0, layout::overhead) + offset);
		} else {
			idx = idxMk(c, "", glm::vec3(layout::overhead, 0.0, layout::overhead) + offset);
		}

		inst->data[c] = idx;
		child[1 + c] = inst->data[c]->_parent;

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
			bound = glm::vec2(inst->x, inst->y);

			break;

		case Z:
			bound = glm::vec2(inst->y, inst->x);

			break;
	}

	Border* scope;
	switch (axis) {
		case X:
			scope = borderMk({
				inst->x * stride[X],
				inst->y * stride[Y]
			}, child, noChild, loc, rot);

			break;

		case Z:
			scope = borderMk({
				inst->y * stride[X],
				inst->x * stride[Y]
			}, child, noChild, loc, rot);

			break;
	}

	inst->_parent = scope->_parent;

	return inst;
}

Array* arrayMk(char* data, unsigned int x, unsigned int y, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Array* _ = (Array*) malloc(sizeof (Array));

	_->x = x;
	_->y = y;

	unsigned int noChild = 1 + (_->x * _->y);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	// identifier
	child[0] = nullptr;

	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
	}

	// data
	_->data = (Idx**) malloc(_->x * _->y * sizeof (Idx*));

	glm::vec2 stride = glm::vec2(layout::item(layout::scoped(layout::idx[X])), layout::item(layout::scoped(layout::idx[Z])));

	int c = 0;
	for (int j = 0; j < _->y; j++) {
		for (int i = 0; i < _->x; i++) {
			Idx* idx;

			glm::vec3 offset = glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(i * stride[X], 0.0, j * stride[Y]);

			if (data[c]) {
				idx = idxMk(c, &data[c], 1, "", offset);
			} else {
				idx = idxMk(c, "", offset);
			}

			_->data[c] = idx;
			child[1 + c] = _->data[c]->_parent;

			c++;
		}
	}

	Border* scope = borderMk({
		_->x * stride[X],
		_->y * stride[Y]
	}, child, noChild, loc, rot);

	_->_parent = scope->_parent;

	return _;
}

void arrayDel(Array* inst) {
	for (int y = 0; y < inst->y; y++) {
		for (int x = 0; x < inst->x; x++) {
			idxDel(inst->data[(y * inst->x) + x]);
		}
	}
	free(inst->data);

	objDel(inst->_parent);

	free(inst);
}

void arrayIns(Array* inst, unsigned int x, unsigned int y, Cont* byte) {
	int i = util::math::idx::array({
		x,
		y
	}, {
		inst->x,
		inst->y
	});

	Idx* idx = inst->data[i];

	idxPush(idx, byte);

	// transform
	glm::vec2 center = layout::center({
		layout::idx[X],
		layout::idx[Z]
	});
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(center[X], 0.0, center[Y]) + glm::vec3(0.0, layout::idx[Y] / 2, 0.0));

	idx->data[idx->sz - 1]->_parent->_model = model;

	objAcc(idx->_parent, inst->_parent->_acc);
}

Cont* arrayRm(Array* inst, unsigned int x, unsigned int y) {
	Idx* idx = inst->data[(y * inst->x) + x];

	return idxPop(idx);
}

bool arrayEq(Array* lhs, Array* rhs) {
	int i = 0;
	for (int y = 0; y < lhs->y; y++) {
		for (int x = 0; x < lhs->x; x++) {
			if (!idxEq(lhs->data[i], rhs->data[i])) {
				return false;
			}

			i++;
		}
	}

	return true;
}
