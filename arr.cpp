#include <cstdlib>

#include "arr.h"
#include "border.h"
#include "state.h"
#include "layout.h"
#include "idx.h"
#include "str.h"

Arr* arrMk(Node* data, glm::vec3 loc, glm::vec3 rot) {
	Arr* _ = (Arr*) malloc(sizeof (Arr));

	_->_x = 0;
	_->_depth = 0;
	nodeMax(data, &_->_x);
	nodeDepth(data, &_->_depth);
	_->_y = _->_depth - 1;

	// layout
	GLfloat
		stride[2] = {
			layout::idx[X] + (layout::offset * 2) + (layout::margin * 2 * 2),
			layout::idx[Y] + (layout::offset * 2) + (layout::margin * 2 * 2)
		},

		scale[2] = {
			layout::stroke + (_->_y * (layout::stroke * 2 * 2)) + (_->_x * stride[X]),
			layout::stroke + (_->_y * (layout::stroke * 2 * 2)) + (_->_y * stride[Y])
		};

	Border* parent = borderMk({
		scale[X],
		scale[Y]
	}, loc + glm::vec3(0.0, layout::margin, 0.0), rot);

	_->_parent = parent->_parent;

	_->_parent->_noChild = _->_y + (_->_x * 2 * _->_y) + 1;
	_->_parent->_child = (Obj**) realloc(_->_parent->_child, _->_parent->_noChild * sizeof (Obj*));

	// scope
	int c = 0;
	for (int j = 0; j < _->_y; j++) {
		GLfloat offset = layout::stroke + (layout::margin * 2);

		Border* child = borderMk({
			parent->_sz[X] - (((j + 1) * (layout::offset * 2))) - (layout::margin * 2),
			parent->_sz[Y] - (((j + 1) * (layout::offset * 2))) - (layout::margin * 2)
		}, loc + glm::vec3(0.0, layout::margin, 0.0) + glm::vec3(
			(j + 1) * offset,
			0.0,
			(j + 1) * offset
		), rot);

		_->_parent->_child[c] = child->_parent;

		c++;

		for (int i = 0; i < _->_x; i++) {
			GLfloat offset = (1 + j + 1) * (layout::stroke + (layout::margin * 2));

			Idx* idx = idxMk(i, loc + glm::vec3(0.0, layout::margin, 0.0) + glm::vec3(
				offset + (i * stride[X]),
				0.0,
				offset
			), rot);

			_->_parent->_child[c] = idx->_parent;

			c++;

			// data
			if (data->_data[i]) {
				_->_parent->_child[c] = objMk("container_2x4", "obj", "dir", true, loc + glm::vec3(offset + (i * stride[X]), 2.0, offset) + glm::vec3(-1.0 + -layout::margin + (layout::stroke * 2) + (layout::margin * 2) + (layout::idx[X] / 2), 0.0, -1.0 + -layout::margin + (layout::stroke * 2) + (layout::margin * 2) + (layout::idx[Y] / 2)));
			} else {
				_->_parent->_child[c] = nullptr;
			}

			c++;
		}
	}

	// name
	Str* str = strMk("asdf", loc + glm::vec3(layout::margin, 0.0, -layout::offset));

	_->_parent->_child[_->_y + (_->_x * 2 * _->_y)] = str->_parent;

	return _;
}
