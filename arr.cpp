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

	int no = 0;
	for (int i = 0; i < _->_x; i++) {
		Idx* idx;
		glm::vec3 offset = glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(i * layout::stride[X], 0.0, 0.0);
		if (init[no]) {
			idx = idxMk(no, &init[no], 1, "", offset);
		} else {
			idx = idxMk(no, "", offset);
		}

		_->_data[no] = idx;
		child[1 + no] = _->_data[no]->_parent;

		no++;
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
	} else {
		child[0] = nullptr;
	}

	// scope
	glm::vec2 pad = {
		layout::margin * 2,
		layout::margin * 2
	};

	glm::vec2 sz = {
		(_->_x * layout::stride[X]) + pad[X],
		(_->_y * layout::stride[Z]) + pad[Y]
	};

	Border* scope = borderMk(sz, child, noChild, loc, rot);

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

	int no = 0;
	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(i * layout::stride[X], 0.0, 0.0);
			if (init[no]) {
				idx = idxMk(no, &init[no], 1, "", offset);
			} else {
				idx = idxMk(no, "", offset);
			}

			_->_data[no] = idx;
			child[1 + no] = _->_data[no]->_parent;

			no++;
		}
	}

	// scope
	glm::vec2 pad = {
		layout::margin * 2,
		layout::margin * 2
	};

	glm::vec2 sz = {
		(_->_x * layout::stride[X]) + pad[X],
		(_->_y * layout::stride[Z]) + pad[Y]
	};

	Border* scope = borderMk(sz, child, noChild, loc, rot);

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
	_->_z = z;

	unsigned int noChild = 1 + (_->_x * _->_y * _->_z);
	Obj** child = (Obj**) malloc(noChild * sizeof (Obj*));

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
	child[0] = id->_parent;

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

	Idx* idx;

	int i = 0;
	for (int x = 0; x < 3; x++) {
		glm::vec3 offset = glm::vec3(layout::overhead, 0.0, layout::overhead) + glm::vec3(
			x * (2.0 + (0.16 * 2 * 2 * 2) + (0.16 * 2 * 2) + (0.16 * 2)),
			0.0,
			0.0
		);

		char* asdf = (char*) malloc(3 * sizeof (char));
		asdf[0] = 'T';
		asdf[1] = 'e';
		asdf[2] = 'x';
		idx = idxMk(0, asdf, 3, "", offset);

		_->_data[i] = idx;

		child[1 + i] = _->_data[i]->_parent;

		i++;
	}

	// scope
	glm::vec2 pad = {
		layout::margin * 2,
		layout::margin * 2
	};

	glm::vec2 sz = {
		(_->_x * layout::stride[X]) + pad[X],
		(_->_y * layout::stride[Z]) + pad[Y]
	};

	Border* scope = borderMk(sz, child, noChild, loc, rot);

	_->_parent = scope->_parent;

	glm::vec3 offset = _->_parent->_acc * glm::vec4(glm::vec3(0.0), 1.0);
	for (int i = 0; i < 3; i++) {
		_->_loc[i] = offset[i];
	}

	return _;
}

bool arrEq(Arr* lhs, Arr* rhs) {
	bool _ = true;

	int no = 0;
	for (int y = 0; y < lhs->_y; y++) {
		for (int x = 0; x < lhs->_x; x++) {
			if (!idxEq(lhs->_data[no], rhs->_data[no])) {
				_ = false;
			}

			if (!_) {
				break;
			}

			no++;
		}

		if (!_) {
			break;
		}
	}

	return _;
}
