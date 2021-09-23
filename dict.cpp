#include <cstdlib>

#include "dict.h"
#include "str.h"
#include "layout.h"
#include "border.h"
#include "idx.h"
#include "scn.h"
#include "omni.h"
#include "util.h"

Dict* dictMk(nlohmann::json deser, glm::vec3 loc, glm::vec3 rot) {
	Dict* _ = (Dict*) malloc(sizeof (Dict));
	
	_->_no = 1;
	_->_data = (void**) malloc(_->_no * sizeof (void*));
	_->_type = (unsigned int*) malloc(_->_no * sizeof (unsigned int));

	Obj* child[deser.size()];

	glm::vec2 strideLetter = layout::item(layout::item({
		0.0,
		layout::letter[Y]
	}));

	glm::vec2 strideIdx = layout::bordered({
		layout::idx[X],
		layout::idx[Z]
	});

	glm::vec2 overhead = glm::vec2(layout::overhead, layout::overhead + strideLetter[Y]);

	// data
	int i = 0;
	GLfloat maxX = 0.0;
	GLfloat accY = 0.0;
	for (const auto& entry : deser.items()) {
		// scalar
		if (entry.value().type() == nlohmann::json::value_t::number_unsigned) {
			char init = (int) entry.value();

			Idx* _ = idxMk(0, &init, 1, entry.key(), glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

			child[i] = _->_parent;

			if (strideIdx[X] > maxX) {
				maxX = strideIdx[X];
			}

			accY += strideLetter[Y] + strideIdx[Y];
		}

		// array
		if (entry.value().type() == nlohmann::json::value_t::array) {
			// 1D
			if (entry.value()[0].type() == nlohmann::json::value_t::number_unsigned) {
				CBuff init = util::json::array::array(entry.value());

				Array* _ = arrayMk((char*) init._ptr, init._x, entry.key(), X, glm::vec3(overhead[X], 0.0, overhead[Y]) + glm::vec3(0.0, 0.0, accY));

				child[i] = _->_parent;

				if (init._x * strideIdx[X] > maxX) {
					maxX = init._x * strideIdx[X];
				}

				accY += strideLetter[Y] + strideIdx[Y];
			}
		}

		i++;
	}

	// scope
	Border* scope = borderMk({
		maxX,
		accY
	}, child, sizeof child / sizeof *child);

	_->_parent = scope->_parent;

	return _;
}

void dictDel(Dict* dict) {
	objDel(dict->_parent);

	free(dict);
}

bool dictEq(Dict* lhs, Dict* rhs) {
	bool _ = true;

	for (int i = 0; i < lhs->_no; i++) {
		if (!_) {
			break;
		}

		switch (lhs->_type[i]) {
			case omni::SCALAR:
				if (!idxEq((Idx*) lhs->_data[i], (Idx*) rhs->_data[i])) {
					_ = false;
				}

				break;

			case omni::ARRAY:
				if (!arrayEq((Array*) lhs->_data[i], (Array*) rhs->_data[i])) {
					_ = false;
				}

				break;
		}
	}

	return _;
}
