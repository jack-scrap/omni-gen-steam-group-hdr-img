#include <cstdlib>

#include "array.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"

Array* arrayMk(char* init, unsigned int x, std::string name, unsigned int axis, glm::vec3 loc, glm::vec3 rot) {
	Array* _ = (Array*) malloc(sizeof (Array));

	_->_x = x;
	_->_y = 1;

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * sizeof (Idx*));

	unsigned int noChild = 1 + (_->_x * _->_y);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	glm::vec2 stride = glm::vec2(layout::item(layout::bordered(layout::idx[X])), layout::item(layout::bordered(layout::idx[Z])));

	int c = 0;
	for (int i = 0; i < _->_x; i++) {
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

		_->_data[c] = idx;
		child[1 + c] = _->_data[c]->_parent;

		c++;
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
	} else {
		child[0] = nullptr;
	}

	glm::vec2 bound;
	switch (axis) {
		case X:
			bound = glm::vec2(_->_x, _->_y);

			break;

		case Z:
			bound = glm::vec2(_->_y, _->_x);

			break;
	}

	Border* scope;
	switch (axis) {
		case X:
			scope = borderMk({
				_->_x * stride[X],
				_->_y * stride[Y]
			}, child, noChild, loc, rot);

			break;

		case Z:
			scope = borderMk({
				_->_y * stride[X],
				_->_x * stride[Y]
			}, child, noChild, loc, rot);

			break;
	}

	_->_parent = scope->_parent;

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int a = 0; a < 3; a++) {
		_->_loc[a] = offset[a];
	}

	return _;
}

Array* arrayMk(char* init, unsigned int x, unsigned int y, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Array* _ = (Array*) malloc(sizeof (Array));

	_->_x = x;
	_->_y = y;

	unsigned int noChild = 1 + (_->_x * _->_y);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
	} else {
		child[0] = nullptr;
	}

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * sizeof (Idx*));

	glm::vec2 stride = glm::vec2(layout::item(layout::bordered(layout::idx[X])), layout::item(layout::bordered(layout::idx[Z])));

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
		_->_loc[a] = offset[a];
	}

	return _;
}

void arrayDel(Array* array) {
	for (int y = 0; y < array->_y; y++) {
		for (int x = 0; x < array->_x; x++) {
			idxDel(array->_data[(y * array->_x) + x]);
		}
	}
	free(array->_data);

	objDel(array->_parent);

	free(array);
}

bool arrayEq(Array* lhs, Array* rhs) {
	bool _ = true;

	int i = 0;
	for (int y = 0; y < lhs->_y; y++) {
		for (int x = 0; x < lhs->_x; x++) {
			if (!idxEq(lhs->_data[i], rhs->_data[i])) {
				_ = false;
			}

			if (!_) {
				break;
			}

			i++;
		}

		if (!_) {
			break;
		}
	}

	return _;
}
