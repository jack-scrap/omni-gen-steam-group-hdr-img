#include <cstdlib>

#include "arr.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"

Arr* arrMk(char* data, unsigned int sz, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = sz;
	_->_y = 1;

	_->_data = (Idx**) malloc(_->_y * _->_x * sizeof (Idx*));

	// layout
	GLfloat
		stride[2] = {
			layout::idx[X] + (layout::offset * 2) + (layout::margin * 2 * 2),
			layout::idx[Y] + (layout::offset * 2) + (layout::margin * 2 * 2)
		},

		scale[2] = {
			(_->_y * layout::stroke) + (_->_x * stride[X]),
			(_->_y * layout::stroke) + (_->_y * stride[Y])
		};

	// data
	Obj** child = (Obj**) malloc(0);
	unsigned int noChild = 1;

	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * stride[X], 0.0, j * stride[Y]);
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
	}, child, noChild, loc + glm::vec3(0.0, layout::margin, 0.0), rot);

	_->_parent = parent->_parent;

	return _;
}

Arr* arrMk(char* data, unsigned int x, unsigned int y, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = x;
	_->_y = y;

	_->_data = (Idx**) malloc(_->_y * _->_x * sizeof (Idx*));

	// layout
	GLfloat
		stride[2] = {
			layout::idx[X] + (layout::offset * 2) + (layout::margin * 2 * 2),
			layout::idx[Y] + (layout::offset * 2) + (layout::margin * 2 * 2)
		},

		scale[2] = {
			(_->_y * layout::stroke) + (_->_x * stride[X]),
			(_->_y * layout::stroke) + (_->_y * stride[Y])
		};

	// data
	Obj** child = (Obj**) malloc(0);
	unsigned int noChild = 1;

	for (int j = 0; j < _->_y; j++) {
		for (int i = 0; i < _->_x; i++) {
			Idx* idx;
			glm::vec3 offset = glm::vec3(layout::stroke * 2, 0.0, layout::stroke * 2) + glm::vec3(i * stride[X], 0.0, j * stride[Y]);
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
	}, child, noChild, loc + glm::vec3(0.0, layout::margin, 0.0), rot);

	_->_parent = parent->_parent;

	return _;
}
