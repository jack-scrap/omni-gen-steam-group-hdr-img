#include <cstdlib>

#include "arr.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"

Arr* arrMk(char* init, unsigned int x, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = x;
	_->_y = 1;
	_->_z = 1;

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

	unsigned int noChild = 1 + (_->_x * _->_y * _->_z);
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

	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

Arr* arrMk(char* init, unsigned int x, unsigned int y, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = x;
	_->_y = y;
	_->_z = 1;

	unsigned int noChild = 1 + (_->_x * _->_y * _->_z);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
	child[0] = id->_parent;

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

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

	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

Arr* arrMk(char* init, unsigned int x, unsigned int y, unsigned int z, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = x;
	_->_y = y;
	_->_z = 1;

	unsigned int noChild = 1 + (_->_x * _->_y * _->_z);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
	child[0] = id->_parent;

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

	int c = 0;
	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			int start = ((j * _->_y) + i) * _->_x;

			char* vec = (char*) malloc(z * sizeof (char));
			for (int i = 0; i < z; i++) {
				vec[i] = init[start + i];
			}

			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Z]);
			if (init[c]) {
				idx = idxMk(c, vec, z, "", offset);
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

	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

bool arrEq(Arr* lhs, Arr* rhs) {
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
