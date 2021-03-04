#include <cstdlib>

#include "arr.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"

Arr* arrMk(Node* data, std::string name, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = 0;
	_->_depth = 0;
	nodeMax(data, &_->_x);
	nodeDepth(data, &_->_depth);
	_->_y = _->_depth - 1;

	_->_data = (Idx**) malloc(_->_x * sizeof (Idx*));

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

	Border* parent = borderMk({
		scale[X],
		scale[Y]
	}, glm::vec3(0.0, layout::margin, 0.0), rot);

	_->_parent = parent->_parent;

	_->_parent->_noChild = _->_y + (_->_x * _->_y) + 1;
	_->_parent->_child = (Obj**) realloc(_->_parent->_child, _->_parent->_noChild * sizeof (Obj*));

	// scope
	int c = 0;
	for (int j = 0; j < _->_y; j++) {
		GLfloat offset = layout::stroke + (layout::margin * 2);

		for (int i = 0; i < _->_x; i++) {
			GLfloat offset = (j + 1) * (layout::stroke + (layout::margin * 2));

			Idx* idx;
			if (data->_data[i]) {
				idx = idxMk(i, data->_data[i], glm::vec3(0.0, layout::margin, 0.0) + glm::vec3(offset + (i * stride[X]), 0.0, offset));
			} else {
				idx = idxMk(i, glm::vec3(0.0, layout::margin, 0.0) + glm::vec3(
					offset + (i * stride[X]),
					0.0,
					offset
				));
			}

			_->_data[i] = idx;

			_->_parent->_child[c] = idx->_parent;

			c++;
		}
	}

	// name
	Str* str = strMk(name, glm::vec3(layout::margin, 0.0, -layout::offset), glm::vec3(0.0, M_PI / 2, 0.0));

	_->_parent->_child[_->_y + (_->_x * _->_y)] = str->_parent;

	return _;
}
