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

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

	Obj** child = (Obj**) malloc((1 + (_->_x * _->_y * _->_z)) * sizeof (Obj*));
	unsigned int noChild = 1;

	int no = 0;
	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Z]);
			if (init[no]) {
				idx = idxMk(no, init[no], "", offset);
			} else {
				idx = idxMk(no, "", offset);
			}

			_->_data[no] = idx;

			noChild++;
			child[noChild - 1] = _->_data[no]->_parent;

			no++;
		}
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[0] = id->_parent;
	} else {
		child[0] = nullptr;
	}

	// scope
	Border* scope = borderMk({
		layout::stroke + (_->_x * layout::stride[X]),
		layout::stroke + (_->_y * layout::stride[Z])
	}, child, noChild, loc, rot);

	_->_parent = scope->_parent;

	return _;
}

Arr* arrMk(char* init, unsigned int x, unsigned int y, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = x;
	_->_y = y;
	_->_z = 1;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

	Obj** child = (Obj**) malloc((1 + (_->_x * _->_y * _->_z)) * sizeof (Obj*));
	unsigned int noChild = 1;

	int no = 0;
	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Z]);
			if (init[no]) {
				idx = idxMk(no, init[no], "", offset);
			} else {
				idx = idxMk(no, "", offset);
			}

			_->_data[no] = idx;

			noChild++;
			child[noChild -1] = _->_data[no]->_parent;

			no++;
		}
	}

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
	child[0] = id->_parent;

	Border* scope = borderMk({
		layout::stroke + (_->_x * layout::stride[X]),
		layout::stroke + (_->_y * layout::stride[Z])
	}, child, noChild, loc, rot);

	_->_parent = scope->_parent;

	return _;
}

Arr* arrMk(char* init, unsigned int x, unsigned int y, unsigned int z, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = x;
	_->_y = y;
	_->_z = z;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	// data
	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

	Obj** child = (Obj**) malloc((1 + (_->_x * _->_y * _->_z)) * sizeof (Obj*));
	unsigned int noChild = 1;

	int no = 0;
	for (int k = 0; k < _->_z; k++) {
		for (int j = 0; j < _->_y; j++) {
			for (int i = 0; i < _->_x; i++) {
				Idx* idx;
				glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Z]);
				if (init[no]) {
					idx = idxMk(no, init[no], "", offset);
				} else {
					idx = idxMk(no, "", offset);
				}

				_->_data[no] = idx;

				noChild++;
				child[noChild - 1] = _->_data[no]->_parent;

				no++;
			}
		}
	}

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
	child[0] = id->_parent;

	// scope
	Border* scope = borderMk({
		layout::stroke + (_->_x * layout::stride[X]),
		layout::stroke + (_->_y * layout::stride[Z])
	}, child, noChild, loc, rot);

	_->_parent = scope->_parent;

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
