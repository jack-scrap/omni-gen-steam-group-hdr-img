#include <cstdlib>

#include "arr.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"

Arr* arrMk(char* init, unsigned int x, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_ptr = init;
	_->_x = x;
	_->_y = 1;
	_->_z = 1;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	_->_data = (Idx**) malloc(_->_x * _->_y * sizeof (Idx*));

	// data
	Obj** child = (Obj**) malloc(0);
	unsigned int noChild = 1;

	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Z]);
			if (init[(j * _->_y) + i]) {
				idx = idxMk(i, init[(j * _->_y) + i], "", offset);
			} else {
				idx = idxMk((j * _->_y) + i, "", offset);
			}

			_->_data[(j * _->_y) + i] = idx;

			noChild++;
			child = (Obj**) realloc(child, noChild * sizeof (Obj*));
			child[(j * _->_y) + i] = _->_data[(j * _->_y) + i]->_parent;
		}
	}

	// identifier
	if (!name.empty()) {
		Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
		child[noChild - 1] = id->_parent;
	} else {
		child[noChild - 1] = nullptr;
	}

	// scope
	Border* scope = borderMk({
		layout::stroke + (_->_x * layout::stride[X]),
		layout::stroke + (_->_y * layout::stride[Z])
	}, child, noChild, loc + glm::vec3(0.0), rot);

	_->_parent = scope->_parent;

	return _;
}

Arr* arrMk(char* init, unsigned int x, unsigned int y, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_ptr = init;
	_->_x = x;
	_->_y = y;
	_->_z = 1;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	_->_data = (Idx**) malloc(_->_x * _->_y * sizeof (Idx*));

	// data
	Obj** child = (Obj**) malloc(0);
	unsigned int noChild = 1;

	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Z]);
			if (init[(j * _->_y) + i]) {
				idx = idxMk(i, init[(j * _->_y) + i], "", offset);
			} else {
				idx = idxMk((j * _->_y) + i, "", offset);
			}

			_->_data[(j * _->_y) + i] = idx;

			noChild++;
			child = (Obj**) realloc(child, noChild * sizeof (Obj*));
			child[(j * _->_y) + i] = _->_data[(j * _->_y) + i]->_parent;
		}
	}

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -(layout::margin * 2)));
	child[noChild - 1] = id->_parent;

	Border* scope = borderMk({
		layout::stroke + (_->_x * layout::stride[X]),
		layout::stroke + (_->_y * layout::stride[Z])
	}, child, noChild, loc + glm::vec3(0.0), rot);

	_->_parent = scope->_parent;

	return _;
}

Arr* arrMk(char* init, unsigned int x, unsigned int y, unsigned int z, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_ptr = init;
	_->_x = x;
	_->_y = y;
	_->_z = z;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

	// data
	Obj** child = (Obj**) malloc(0);
	unsigned int noChild = 1;

	int no = 0;
	for (int k = 0; k < _->_z; k++) {
		for (int j = 0; j < _->_y; j++) {
			for (int i = 0; i < _->_x; i++) {
				Idx* idx;
				glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Z]);
				if (init[no]) {
					idx = idxMk(i, init[no], "", offset);
				} else {
					idx = idxMk(init[no], "", offset);
				}

				_->_data[no] = idx;

				noChild++;
				child = (Obj**) realloc(child, noChild * sizeof (Obj*));
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
	}, child, noChild, loc + glm::vec3(0.0), rot);

	_->_parent = scope->_parent;

	return _;
}

bool arrEq(Arr* lhs, Arr* rhs) {
	bool _ = true;

	for (int y = 0; y < lhs->_y; y++) {
		for (int x = 0; x < lhs->_x; x++) {
			int idx = (y * lhs->_x) + x;

			if (!idxEq(lhs->_data[idx], rhs->_data[idx])) {
				_ = false;
			}

			if (!_) {
				break;
			}
		}

		if (!_) {
			break;
		}
	}

	return _;
}
