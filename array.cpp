#include <cstdlib>

#include "array.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"

Array* arrayMk(char* init, unsigned int x, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Array* _ = (Array*) malloc(sizeof (Array));

	_->_x = x;
	_->_y = 1;

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * sizeof (Idx*));

	unsigned int noChild = 1 + (_->_x * _->_y);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	int c = 0;
	for (int i = 0; i < _->_x; i++) {
		Idx* idx;
		glm::vec3 offset = glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(i * layout::stride[X], 0.0, 0.0);
		if (init[c]) {
			idx = idxMk(c, &init[c], 1, "", offset);
		} else {
			idx = idxMk(c, "", offset);
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

	Border* scope = borderMk(layout::sz({
		_->_x,
		_->_y
	}), child, noChild, loc, rot);

	_->_parent = scope->_parent;

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
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
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
	child[0] = id->_parent;

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * sizeof (Idx*));

	int c = 0;
	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Z]);
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

	Border* scope = borderMk(layout::sz({
		_->_x,
		_->_y
	}), child, noChild, loc, rot);

	_->_parent = scope->_parent;

	// offset
	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
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
