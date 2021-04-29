#include <cstdlib>

#include "data.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"

Data* dataMk(char* data, unsigned int sz, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Data* _ = (Data*) malloc(sizeof (Data));

	_->_ptr = data;
	_->_x = sz;
	_->_y = 1;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	_->_data = (Idx**) malloc(_->_x * _->_y * sizeof (Idx*));

	// layout
	GLfloat scale[2] = {
		(_->_y * layout::stroke) + (_->_x * layout::stride[X]),
		(_->_y * layout::stroke) + (_->_y * layout::stride[Y])
	};

	// data
	Obj** child = (Obj**) malloc(0);
	unsigned int noChild = 1;

	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Y]);
			if (data[(j * _->_y) + i]) {
				idx = idxMk(i, data[(j * _->_y) + i], offset);
			} else {
				idx = idxMk((j * _->_y) + i, offset);
			}

			_->_data[(j * _->_y) + i] = idx;

			noChild++;
			child = (Obj**) realloc(child, noChild * sizeof (Obj*));
			child[(j * _->_y) + i] = _->_data[(j * _->_y) + i]->_parent;
		}
	}

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -layout::margin * 2));
	child[noChild - 1] = id->_parent;

	Border* parent = borderMk({
		scale[X],
		scale[Y]
	}, child, noChild, loc + glm::vec3(0.0, 0.0, 0.0), rot);

	_->_parent = parent->_parent;

	return _;
}

Data* dataMk(char* data, unsigned int x, unsigned int y, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Data* _ = (Data*) malloc(sizeof (Data));

	_->_ptr = data;
	_->_x = x;
	_->_y = y;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	_->_data = (Idx**) malloc(_->_y * _->_x * sizeof (Idx*));

	// layout
	GLfloat scale[2] = {
		(_->_y * layout::stroke) + (_->_x * layout::stride[X]),
		(_->_y * layout::stroke) + (_->_y * layout::stride[Y])
	};

	// data
	Obj** child = (Obj**) malloc(0);
	unsigned int noChild = 1;

	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Y]);
			if (data[(j * _->_y) + i]) {
				idx = idxMk(i, data[(j * _->_y) + i], offset);
			} else {
				idx = idxMk((j * _->_y) + i, offset);
			}

			_->_data[(j * _->_y) + i] = idx;

			noChild++;
			child = (Obj**) realloc(child, noChild * sizeof (Obj*));
			child[(j * _->_y) + i] = _->_data[(j * _->_y) + i]->_parent;
		}
	}

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -layout::margin * 2));
	child[noChild - 1] = id->_parent;

	Border* parent = borderMk({
		scale[X],
		scale[Y]
	}, child, noChild, loc + glm::vec3(0.0, 0.0, 0.0), rot);

	_->_parent = parent->_parent;

	return _;
}

Data* dataMk(char* data, unsigned int x, unsigned int y, unsigned int z, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Data* _ = (Data*) malloc(sizeof (Data));

	_->_ptr = data;
	_->_x = x;
	_->_y = y;
	_->_z = z;

	for (int i = 0; i < 3; i++) {
		_->_loc[i] = loc[i];
	}

	_->_data = (Idx**) malloc(_->_x * _->_y * _->_z * sizeof (Idx*));

	// layout
	GLfloat scale[3] = {
		(_->_y * layout::stroke) + (_->_x * layout::stride[X]),
		(_->_y * layout::stroke) + (_->_y * layout::stride[Y]),
		(_->_z * layout::stroke) + (_->_z * layout::stride[Z])
	};

	// data
	Obj** child = (Obj**) malloc(0);
	unsigned int noChild = 1;

	for (int k = 0; k < _->_y; k++) {
		for (int j = 0; j < _->_y; j++) {
			for (int i = 0; i < _->_x; i++) {
				Idx* idx;
				glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * layout::stride[X], 0.0, j * layout::stride[Y]);
				if (data[(k * (j * _->_y)) + i]) {
					idx = idxMk(i, data[(k * (j * _->_y)) + i], offset);
				} else {
					idx = idxMk((k * (j * _->_y)) + i, offset);
				}

				_->_data[(k * (j * _->_y)) + i] = idx;

				noChild++;
				child = (Obj**) realloc(child, noChild * sizeof (Obj*));
				child[(k * (j * _->_y)) + i] = _->_data[(k * (j * _->_y)) + i]->_parent;
			}
		}
	}

	// identifier
	Str* id = strMk(name, glm::vec3(0.0, 0.0, -layout::margin * 2));
	child[noChild - 1] = id->_parent;

	Border* parent = borderMk({
			scale[X],
			scale[Y]
			}, child, noChild, loc + glm::vec3(0.0, 0.0, 0.0), rot);

	_->_parent = parent->_parent;

	return _;
}
